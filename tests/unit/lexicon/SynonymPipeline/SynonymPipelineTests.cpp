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

#include <unordered_map>

#include "Lexicon/SynonymPipeline.h"

using namespace ADS;
using namespace ADS::Lexicon;

namespace {

    /// Minimal ILexiconLookup stub — lets each test control exactly which
    /// forms/stems are "known" without needing a full Lexicon.
    class FakeLookup final : public ILexiconLookup {
    public:
        std::unordered_map<std::string, LexEntryId> entriesByForm;
        std::unordered_map<std::string, std::vector<LexEntryId>> entriesByStem;

        [[nodiscard]] LexEntryId findEntryId(std::string_view form, const LanguageCode& lang) const override {
            const auto it = entriesByForm.find(lang + ":" + std::string(form));
            return it != entriesByForm.end() ? it->second : INVALID_ENTRY_ID;
        }

        [[nodiscard]] std::vector<LexEntryId> findByStem(std::string_view stemValue,
                                                           const LanguageCode& lang,
                                                           LexEntryId excluding) const override {
            const auto it = entriesByStem.find(lang + ":" + std::string(stemValue));
            if (it == entriesByStem.end()) return {};
            std::vector<LexEntryId> result;
            for (const LexEntryId id : it->second) {
                if (id != excluding) result.push_back(id);
            }
            return result;
        }
    };

} // namespace

TEST(SynonymPipeline, Layer1_RootNotInLexicon_DiscardsMatchEntirely)
{
    // "recabar" -> prefix "re" -> candidate root "cabar", which does not
    // exist — must be discarded, not proposed at lower confidence.
    FakeLookup lookup;

    const auto links = SynonymPipeline::run("recabar", 5, "es_ES", lookup);

    EXPECT_TRUE(links.empty());
}

TEST(SynonymPipeline, Layer1_RootExists_ProposesHighConfidenceLink)
{
    FakeLookup lookup;
    lookup.entriesByForm["es_ES:coger"] = 3;

    const auto links = SynonymPipeline::run("recoger", 5, "es_ES", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 3u);
    EXPECT_FLOAT_EQ(links[0].confidence, SynonymPipeline::LAYER1_CONFIDENCE);
    EXPECT_FALSE(links[0].confirmed);
}

TEST(SynonymPipeline, Layer2_SharedStem_ProposesModerateConfidenceLinks)
{
    FakeLookup lookup;
    lookup.entriesByStem["es_ES:tomar"] = {20, 21};

    const auto links = SynonymPipeline::run("tomar", 10, "es_ES", lookup);

    ASSERT_EQ(links.size(), 2u);
    for (const auto& link : links) {
        EXPECT_FLOAT_EQ(link.confidence, SynonymPipeline::LAYER2_CONFIDENCE);
        EXPECT_FALSE(link.confirmed);
    }
}

TEST(SynonymPipeline, Layer2_ExcludesTheEntryBeingProcessed)
{
    FakeLookup lookup;
    lookup.entriesByStem["en_US:key"] = {1, 2};

    const auto links = SynonymPipeline::run("key", 1, "en_US", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 2u);
}

TEST(SynonymPipeline, Layer1AndLayer2_SameTargetIsNotDuplicated)
{
    FakeLookup lookup;
    lookup.entriesByForm["es_ES:coger"]  = 3;
    lookup.entriesByStem["es_ES:recoger"] = {3};

    const auto links = SynonymPipeline::run("recoger", 5, "es_ES", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 3u);
    EXPECT_FLOAT_EQ(links[0].confidence, SynonymPipeline::LAYER1_CONFIDENCE); // layer 1 wins
}

TEST(SynonymPipeline, NoAffixNoSharedStem_ReturnsEmpty)
{
    FakeLookup lookup;

    const auto links = SynonymPipeline::run("unique", 1, "en_US", lookup);

    EXPECT_TRUE(links.empty());
}

TEST(SynonymPipeline, Layer1_GermanPrefix_ProposesLink)
{
    FakeLookup lookup;
    lookup.entriesByForm["de_DE:stehen"] = 9;

    const auto links = SynonymPipeline::run("verstehen", 5, "de_DE", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 9u);
    EXPECT_FLOAT_EQ(links[0].confidence, SynonymPipeline::LAYER1_CONFIDENCE);
}

TEST(SynonymPipeline, Layer1_FrenchPrefix_ProposesLink)
{
    FakeLookup lookup;
    lookup.entriesByForm["fr_FR:faire"] = 9;

    const auto links = SynonymPipeline::run("refaire", 5, "fr_FR", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 9u);
}

TEST(SynonymPipeline, Layer1_PortuguesePrefix_ProposesLink)
{
    FakeLookup lookup;
    lookup.entriesByForm["pt_PT:fazer"] = 9;

    const auto links = SynonymPipeline::run("desfazer", 5, "pt_PT", lookup);

    ASSERT_EQ(links.size(), 1u);
    EXPECT_EQ(links[0].target, 9u);
}

TEST(SynonymPipeline, Run_LanguageCodeWithoutRegion_StillRuns)
{
    // Exercises the branch of the internal baseLang() helper that finds no
    // '_' separator and returns the code as-is (e.g. a bare "eo").
    FakeLookup lookup;

    const auto links = SynonymPipeline::run("word", 1, "eo", lookup);

    EXPECT_TRUE(links.empty());
}
