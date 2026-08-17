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

#include <cstdio>

#include "Lexicon/FallbackNLPBackend.h"
#include "Lexicon/Lexicon.h"

using namespace ADS;
using namespace ADS::Lexicon;

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

TEST(Lexicon, Record_NewLemma_CreatesEntry)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* entry = lexicon.record(makeToken("key", "key", "en_US"));

    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->canonical, "key");
    EXPECT_EQ(entry->lang, "en_US");
    EXPECT_EQ(entry->rawCount(), 1u);
    EXPECT_EQ(entry->types(), WordTypeBits::Noun);
}

TEST(Lexicon, Record_SameLemmaTwice_UpdatesExistingEntry)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* first  = lexicon.record(makeToken("key", "key", "en_US"));
    LexEntry* second = lexicon.record(makeToken("key", "keys", "en_US", WordTypeBits::Noun));

    EXPECT_EQ(first, second);
    EXPECT_EQ(first->rawCount(), 2u);
}

TEST(Lexicon, Record_ObservesAmbiguousTypesAcrossOccurrences)
{
    ADS::Lexicon::Lexicon lexicon;

    lexicon.record(makeToken("abrir", "abierta", "es_ES", WordTypeBits::Adjective));
    LexEntry* entry = lexicon.record(makeToken("abrir", "abre", "es_ES", WordTypeBits::Verb));

    EXPECT_TRUE(entry->isAmbiguous());
    EXPECT_EQ(entry->types(), WordTypeBits::Adjective | WordTypeBits::Verb);
}

TEST(Lexicon, Record_PunctuationToken_IsFilteredOut)
{
    ADS::Lexicon::Lexicon lexicon;

    NLPToken punctuation;
    punctuation.form     = ".";
    punctuation.lemma     = ".";
    punctuation.wordType  = WordTypeBits::Punctuation;
    punctuation.lang      = "en_US";

    EXPECT_EQ(lexicon.record(punctuation), nullptr);
    EXPECT_TRUE(lexicon.getEntries("en_US").empty());
}

TEST(Lexicon, Record_PartitionsEntriesByLanguage)
{
    ADS::Lexicon::Lexicon lexicon;

    lexicon.record(makeToken("key", "key", "en_US"));
    lexicon.record(makeToken("llave", "llave", "es_ES"));

    EXPECT_EQ(lexicon.getEntries("en_US").size(), 1u);
    EXPECT_EQ(lexicon.getEntries("es_ES").size(), 1u);
    EXPECT_EQ(lexicon.findEntryId("llave", "en_US"), INVALID_ENTRY_ID);
    EXPECT_NE(lexicon.findEntryId("llave", "es_ES"), INVALID_ENTRY_ID);
}

TEST(Lexicon, FindById_ResolvesRecordedEntry)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* entry = lexicon.record(makeToken("key", "key", "en_US"));

    EXPECT_EQ(lexicon.findById(entry->id), entry);
    EXPECT_EQ(lexicon.findById(INVALID_ENTRY_ID), nullptr);
}

TEST(Lexicon, Index_HighestFrequencyEntryGetsFirstTokenIndex)
{
    ADS::Lexicon::Lexicon lexicon;

    // First entry recorded gets the highest frequency: 1 occurrence out of
    // a smaller running total at the moment it was accumulated.
    LexEntry* first  = lexicon.record(makeToken("aaa", "aaa", "en_US"));
    LexEntry* second = lexicon.record(makeToken("bbb", "bbb", "en_US"));

    lexicon.index();

    EXPECT_EQ(first->compiledToken(), Token::SINGLE_BYTE_MIN);
    EXPECT_EQ(second->compiledToken(), Token::SINGLE_BYTE_MIN + 1);
}

TEST(Lexicon, Index_AssignsSingleByteThenExtendedTokensAtThe254Boundary)
{
    ADS::Lexicon::Lexicon lexicon;

    constexpr int kEntryCount = 255;
    for (int i = 0; i < kEntryCount; ++i) {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "word%03d", i);
        lexicon.record(makeToken(buffer, buffer, "en_US"));
    }

    lexicon.index();

    const auto& entries = lexicon.getEntries("en_US");
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

TEST(Lexicon, Index_AmbiguousEntry_StillGetsAssignedAToken)
{
    ADS::Lexicon::Lexicon lexicon;

    // Mirrors the documented "abierto" example: observed as both Adjective
    // and Verb across occurrences.
    LexEntry* entry = lexicon.record(makeToken("abrir", "abierta", "es_ES", WordTypeBits::Adjective));
    lexicon.record(makeToken("abrir", "abre", "es_ES", WordTypeBits::Verb));
    ASSERT_TRUE(entry->isAmbiguous());

    lexicon.index();

    EXPECT_TRUE(entry->isIndexed());
    EXPECT_EQ(entry->compiledToken(), Token::SINGLE_BYTE_MIN);
}

TEST(Lexicon, Feed_UsesBackendToTokeniseAndRecordEveryLexicalWord)
{
    ADS::Lexicon::Lexicon lexicon;
    FallbackNLPBackend backend;

    lexicon.feed("Take the key.", "en_US", backend);

    // "Take", "the", "key" are lexical; "." is punctuation and filtered.
    EXPECT_EQ(lexicon.getEntries("en_US").size(), 3u);
}

TEST(Lexicon, Record_FallbackConfidence_PopulatesStemField)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* entry = lexicon.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));

    EXPECT_EQ(entry->stem, "amig");
}

TEST(Lexicon, Record_FullConfidence_LeavesStemFieldEmpty)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* entry = lexicon.record(makeToken("key", "key", "en_US", WordTypeBits::Noun, 1.0f));

    EXPECT_TRUE(entry->stem.empty());
}

TEST(Lexicon, FindByStem_ReturnsOtherEntriesSharingTheGivenStem)
{
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* a = lexicon.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));
    LexEntry* b = lexicon.record(makeToken("amig-x", "amig-x", "es_ES", WordTypeBits::Noun, 0.6f));
    b->stem = "amig"; // simulate a second entry that stems to the same root as `a`

    const auto siblings = lexicon.findByStem("amig", "es_ES", a->id);

    ASSERT_EQ(siblings.size(), 1u);
    EXPECT_EQ(siblings[0], b->id);
}

TEST(Lexicon, Record_SharedStemAcrossDistinctEntries_CreatesLayer2SynonymLink)
{
    // A pre-existing entry whose canonical form IS already the shared stem
    // ("amig"). When a second, distinct entry ("amigos") is recorded, the
    // synonym pipeline recomputes its stem ("amig") and — via the real
    // Lexicon::findByStem() — finds the first entry as a Layer 2 sibling.
    ADS::Lexicon::Lexicon lexicon;

    LexEntry* root = lexicon.record(makeToken("amig", "amig", "es_ES", WordTypeBits::Noun, 0.6f));
    LexEntry* inflected = lexicon.record(makeToken("amigos", "amigos", "es_ES", WordTypeBits::Noun, 0.6f));

    ASSERT_EQ(inflected->synonyms.size(), 1u);
    EXPECT_EQ(inflected->synonyms[0].target, root->id);
    EXPECT_FLOAT_EQ(inflected->synonyms[0].confidence, 0.6f);
}
