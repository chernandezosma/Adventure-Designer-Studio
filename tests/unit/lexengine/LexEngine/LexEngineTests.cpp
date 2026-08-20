/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>

#include "LexEngine/FallbackNLPBackend.h"
#include "LexEngine/LexEngine.h"

using namespace ADS;
using namespace ADS::LexEngine;

namespace {

    NLPToken makeToken(const std::string& lemma, const std::string& form,
                        const LanguageCode& lang, WordType wordType = WordTypeBits::Noun,
                        float confidence = 1.0f) {
        NLPToken token;
        token.form       = form;
        token.lemma       = lemma;
        token.upos        = "NOUN";
        token.confidence  = confidence;
        token.wordType    = wordType;
        token.lang        = lang;
        return token;
    }

} // namespace

TEST(LexEngine, Record_NewLemma_CreatesEntry)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* entry = lexEngine.record(makeToken("key", "key", "en_US"));

    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->canonical, "key");
    EXPECT_EQ(entry->lang, "en_US");
    EXPECT_EQ(entry->rawCount(), 1u);
    EXPECT_EQ(entry->types(), WordTypeBits::Noun);
}

TEST(LexEngine, Record_SameLemmaTwice_UpdatesExistingEntry)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* first  = lexEngine.record(makeToken("key", "key", "en_US"));
    LexEntry* second = lexEngine.record(makeToken("key", "keys", "en_US", WordTypeBits::Noun));

    EXPECT_EQ(first, second);
    EXPECT_EQ(first->rawCount(), 2u);
}

TEST(LexEngine, Record_ObservesAmbiguousTypesAcrossOccurrences)
{
    ADS::LexEngine::LexEngine lexEngine;

    lexEngine.record(makeToken("abrir", "abierta", "es_ES", WordTypeBits::Adjective));
    LexEntry* entry = lexEngine.record(makeToken("abrir", "abre", "es_ES", WordTypeBits::Verb));

    EXPECT_TRUE(entry->isAmbiguous());
    EXPECT_EQ(entry->types(), WordTypeBits::Adjective | WordTypeBits::Verb);
}

TEST(LexEngine, Record_PunctuationToken_IsFilteredOut)
{
    ADS::LexEngine::LexEngine lexEngine;

    NLPToken punctuation;
    punctuation.form     = ".";
    punctuation.lemma     = ".";
    punctuation.wordType  = WordTypeBits::Punctuation;
    punctuation.lang      = "en_US";

    EXPECT_EQ(lexEngine.record(punctuation), nullptr);
    EXPECT_TRUE(lexEngine.getEntries("en_US").empty());
}

TEST(LexEngine, Record_PartitionsEntriesByLanguage)
{
    ADS::LexEngine::LexEngine lexEngine;

    lexEngine.record(makeToken("key", "key", "en_US"));
    lexEngine.record(makeToken("llave", "llave", "es_ES"));

    EXPECT_EQ(lexEngine.getEntries("en_US").size(), 1u);
    EXPECT_EQ(lexEngine.getEntries("es_ES").size(), 1u);
    EXPECT_EQ(lexEngine.findEntryId("llave", "en_US"), INVALID_ENTRY_ID);
    EXPECT_NE(lexEngine.findEntryId("llave", "es_ES"), INVALID_ENTRY_ID);
}

TEST(LexEngine, FindById_ResolvesRecordedEntry)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* entry = lexEngine.record(makeToken("key", "key", "en_US"));

    EXPECT_EQ(lexEngine.findById(entry->id), entry);
    EXPECT_EQ(lexEngine.findById(INVALID_ENTRY_ID), nullptr);
}

TEST(LexEngine, Index_HighestFrequencyEntryGetsFirstTokenIndex)
{
    ADS::LexEngine::LexEngine lexEngine;

    // First entry recorded gets the highest frequency: 1 occurrence out of
    // a smaller running total at the moment it was accumulated.
    LexEntry* first  = lexEngine.record(makeToken("aaa", "aaa", "en_US"));
    LexEntry* second = lexEngine.record(makeToken("bbb", "bbb", "en_US"));

    lexEngine.index();

    EXPECT_EQ(first->compiledToken(), Token::SINGLE_BYTE_MIN);
    EXPECT_EQ(second->compiledToken(), Token::SINGLE_BYTE_MIN + 1);
}

TEST(LexEngine, Index_AssignsSingleByteThenExtendedTokensAtThe254Boundary)
{
    ADS::LexEngine::LexEngine lexEngine;

    constexpr int kEntryCount = 255;
    for (int i = 0; i < kEntryCount; ++i) {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "word%03d", i);
        lexEngine.record(makeToken(buffer, buffer, "en_US"));
    }

    lexEngine.index();

    const auto& entries = lexEngine.getEntries("en_US");
    ASSERT_EQ(entries.size(), static_cast<std::size_t>(kEntryCount));

    // Insertion order == descending-frequency order, since frequency is
    // recomputed at each accumulate() call against the running total.
    for (int rank = 0; rank < 254; ++rank) {
        EXPECT_TRUE(Token::isSingleByte(entries[rank]->compiledToken()))
            << "rank " << rank << " expected a single-byte token";
    }
    EXPECT_EQ(entries[253]->compiledToken(), Token::SINGLE_BYTE_MAX);
    EXPECT_EQ(entries[254]->compiledToken(), Token::EXTENDED_MIN);
}

TEST(LexEngine, Index_AmbiguousEntry_StillGetsAssignedAToken)
{
    ADS::LexEngine::LexEngine lexEngine;

    // Mirrors the documented "abierto" example: observed as both Adjective
    // and Verb across occurrences.
    LexEntry* entry = lexEngine.record(makeToken("abrir", "abierta", "es_ES", WordTypeBits::Adjective));
    lexEngine.record(makeToken("abrir", "abre", "es_ES", WordTypeBits::Verb));
    ASSERT_TRUE(entry->isAmbiguous());

    lexEngine.index();

    EXPECT_TRUE(entry->isIndexed());
    EXPECT_EQ(entry->compiledToken(), Token::SINGLE_BYTE_MIN);
}

TEST(LexEngine, Feed_UsesBackendToTokeniseAndRecordEveryLexicalWord)
{
    ADS::LexEngine::LexEngine lexEngine;
    FallbackNLPBackend backend;

    lexEngine.feed("Take the key.", "en_US", backend);

    // "Take", "the", "key" are lexical; "." is punctuation and filtered.
    EXPECT_EQ(lexEngine.getEntries("en_US").size(), 3u);
}

TEST(LexEngine, Record_FallbackConfidence_PopulatesStemField)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* entry = lexEngine.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));

    EXPECT_EQ(entry->stem, "amig");
}

TEST(LexEngine, Record_FullConfidence_LeavesStemFieldEmpty)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* entry = lexEngine.record(makeToken("key", "key", "en_US", WordTypeBits::Noun, 1.0f));

    EXPECT_TRUE(entry->stem.empty());
}

TEST(LexEngine, FindByStem_ReturnsOtherEntriesSharingTheGivenStem)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* a = lexEngine.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));
    LexEntry* b = lexEngine.record(makeToken("amig-x", "amig-x", "es_ES", WordTypeBits::Noun, 0.6f));
    b->stem = "amig"; // simulate a second entry that stems to the same root as `a`

    const auto siblings = lexEngine.findByStem("amig", "es_ES", a->id);

    ASSERT_EQ(siblings.size(), 1u);
    EXPECT_EQ(siblings[0], b->id);
}

TEST(LexEngine, Record_SharedStemAcrossDistinctEntries_CreatesLayer2SynonymLink)
{
    // A pre-existing entry whose canonical form IS already the shared stem
    // ("amig"). When a second, distinct entry ("amigos") is recorded, the
    // synonym pipeline recomputes its stem ("amig") and — via the real
    // LexEngine::findByStem() — finds the first entry as a Layer 2 sibling.
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry* root = lexEngine.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));
    LexEntry* inflected = lexEngine.record(makeToken("amigos", "amigos", "es_ES", WordTypeBits::Noun, 0.6f));

    ASSERT_EQ(inflected->synonyms.size(), 1u);
    EXPECT_EQ(inflected->synonyms[0].target, root->id);
    EXPECT_FLOAT_EQ(inflected->synonyms[0].confidence, 0.6f);
}

TEST(LexEngine, GetLanguages_EmptyEngine_ReturnsEmpty)
{
    ADS::LexEngine::LexEngine lexEngine;

    EXPECT_TRUE(lexEngine.getLanguages().empty());
}

TEST(LexEngine, GetLanguages_MultipleLanguages_ReturnsEachOnce)
{
    ADS::LexEngine::LexEngine lexEngine;

    lexEngine.record(makeToken("key", "key", "en_US"));
    lexEngine.record(makeToken("llave", "llave", "es_ES"));
    lexEngine.record(makeToken("keys", "keys", "en_US"));

    auto languages = lexEngine.getLanguages();
    std::sort(languages.begin(), languages.end());

    ASSERT_EQ(languages.size(), 2u);
    EXPECT_EQ(languages[0], "en_US");
    EXPECT_EQ(languages[1], "es_ES");
}

TEST(LexEngine, RestoreEntry_PreservesIdAndRebuildsLookup)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry entry;
    entry.id        = 42;
    entry.lang       = "en_US";
    entry.canonical  = "key";
    entry.restoreCounts(5, 0.5f);

    LexEntry* restored = lexEngine.restoreEntry(entry, "en_US");

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->id, 42u);
    EXPECT_EQ(lexEngine.findById(42), restored);
    EXPECT_EQ(lexEngine.findEntryId("key", "en_US"), 42u);
    ASSERT_EQ(lexEngine.getEntries("en_US").size(), 1u);
}

TEST(LexEngine, RestoreEntry_AdvancesNextIdPastRestoredId)
{
    ADS::LexEngine::LexEngine lexEngine;

    LexEntry entry;
    entry.id        = 100;
    entry.lang       = "en_US";
    entry.canonical  = "key";

    lexEngine.restoreEntry(entry, "en_US");
    LexEntry* fresh = lexEngine.record(makeToken("door", "door", "en_US"));

    EXPECT_GT(fresh->id, 100u);
}
