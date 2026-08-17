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

#include "Lexicon/LexiconSerializer.h"

using namespace ADS;
using namespace ADS::Lexicon;

namespace {

    LexEntry makeSampleEntry() {
        LexEntry entry;
        entry.id        = 3;
        entry.lang       = "es_ES";
        entry.canonical  = "abrir";
        entry.role       = EntryRole::Both;
        entry.observeType(WordTypeBits::Verb);
        entry.observeType(WordTypeBits::Adjective);
        entry.accumulate(700); // raw_count=1, frequency ~ 0.00142857
        entry.synonyms.push_back(SynonymLink{.target = 18, .confidence = 0.9f, .confirmed = false});
        entry.synonyms.push_back(SynonymLink{.target = 42, .confidence = 1.0f, .confirmed = true});
        return entry;
    }

} // namespace

TEST(LexiconSerializer, ToJson_ProducesCanonicalNodeStructure)
{
    const LexEntry entry = makeSampleEntry();

    const nlohmann::json json = LexiconSerializer::toJson(entry);

    EXPECT_EQ(json.at("id").get<LexEntryId>(), 3u);
    EXPECT_EQ(json.at("lang").get<std::string>(), "es_ES");
    EXPECT_EQ(json.at("canonical").get<std::string>(), "abrir");
    EXPECT_EQ(json.at("role").get<uint8_t>(), static_cast<uint8_t>(EntryRole::Both));
    EXPECT_EQ(json.at("types").get<WordType>(), WordTypeBits::Verb | WordTypeBits::Adjective);
    EXPECT_EQ(json.at("raw_count").get<uint32_t>(), 1u);

    const auto synonyms = json.at("synonyms").get<std::vector<LexEntryId>>();
    ASSERT_EQ(synonyms.size(), 2u);
    EXPECT_EQ(synonyms[0], 18u);
    EXPECT_EQ(synonyms[1], 42u);

    ASSERT_EQ(json.at("synonym_meta").size(), 2u);
    EXPECT_FLOAT_EQ(json.at("synonym_meta")[0].at("confidence").get<float>(), 0.9f);
    EXPECT_FALSE(json.at("synonym_meta")[0].at("confirmed").get<bool>());
    EXPECT_TRUE(json.at("synonym_meta")[1].at("confirmed").get<bool>());
}

TEST(LexiconSerializer, RoundTrip_PreservesPersistedFields)
{
    const LexEntry original = makeSampleEntry();

    const LexEntry restored = LexiconSerializer::fromJson(LexiconSerializer::toJson(original));

    EXPECT_EQ(restored.id, original.id);
    EXPECT_EQ(restored.lang, original.lang);
    EXPECT_EQ(restored.canonical, original.canonical);
    EXPECT_EQ(restored.role, original.role);
    EXPECT_EQ(restored.types(), original.types());
    EXPECT_EQ(restored.rawCount(), original.rawCount());
    EXPECT_FLOAT_EQ(restored.frequency(), original.frequency());

    ASSERT_EQ(restored.synonyms.size(), 2u);
    EXPECT_EQ(restored.synonyms[0].target, 18u);
    EXPECT_FLOAT_EQ(restored.synonyms[0].confidence, 0.9f);
    EXPECT_FALSE(restored.synonyms[0].confirmed);
    EXPECT_EQ(restored.synonyms[1].target, 42u);
    EXPECT_TRUE(restored.synonyms[1].confirmed);
}

TEST(LexiconSerializer, RoundTrip_DoesNotReconstructDominantTypeOrDiagnostics)
{
    // dominant_type, type_counts, upos_raw, feats, stem, compiled_token are
    // documented as NOT persisted — verify the round trip does not silently
    // fabricate them.
    const LexEntry original = makeSampleEntry();

    const LexEntry restored = LexiconSerializer::fromJson(LexiconSerializer::toJson(original));

    EXPECT_EQ(restored.dominantType(), WordTypeBits::None);
    EXPECT_TRUE(restored.upos_raw.empty());
    EXPECT_TRUE(restored.stem.empty());
    EXPECT_EQ(restored.compiledToken(), Token::UNASSIGNED);
}

TEST(LexiconSerializer, FromJson_MissingSynonymMeta_DefaultsToManuallyConfirmed)
{
    nlohmann::json json;
    json["id"]        = 1;
    json["lang"]      = "en_US";
    json["canonical"] = "take";
    json["role"]      = static_cast<uint8_t>(EntryRole::Both);
    json["types"]     = WordTypeBits::Verb;
    json["frequency"] = 0.1f;
    json["raw_count"] = 5;
    json["synonyms"]  = {7};
    // synonym_meta intentionally absent.

    const LexEntry restored = LexiconSerializer::fromJson(json);

    ASSERT_EQ(restored.synonyms.size(), 1u);
    EXPECT_EQ(restored.synonyms[0].target, 7u);
    EXPECT_FLOAT_EQ(restored.synonyms[0].confidence, 1.0f);
    EXPECT_TRUE(restored.synonyms[0].confirmed);
}
