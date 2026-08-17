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

#include "Lexicon/FallbackNLPBackend.h"

using namespace ADS;
using namespace ADS::Lexicon;

TEST(FallbackNLPBackend, Analyse_SetsFallbackConfidenceAndOtherWordType)
{
    FallbackNLPBackend backend;

    const auto tokens = backend.analyse("key", "en_US");

    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_FLOAT_EQ(tokens[0].confidence, 0.6f);
    EXPECT_EQ(tokens[0].wordType, WordTypeBits::Other);
    EXPECT_EQ(tokens[0].lang, "en_US");
}

TEST(FallbackNLPBackend, Analyse_FiltersPunctuationAsNonLexical)
{
    FallbackNLPBackend backend;

    const auto tokens = backend.analyse("Take the key.", "en_US");

    // "Take", "the", "key" are lexical; "." is punctuation and non-lexical.
    int lexicalCount = 0;
    bool sawPunctuation = false;
    for (const auto& token : tokens) {
        if (token.isLexical()) ++lexicalCount;
        if (token.wordType == WordTypeBits::Punctuation) sawPunctuation = true;
    }

    EXPECT_EQ(lexicalCount, 3);
    EXPECT_TRUE(sawPunctuation);
}

TEST(FallbackNLPBackend, Analyse_WhitespaceSeparatesWords)
{
    FallbackNLPBackend backend;

    const auto tokens = backend.analyse("open   the  door", "en_US");

    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].form, "open");
    EXPECT_EQ(tokens[1].form, "the");
    EXPECT_EQ(tokens[2].form, "door");
}

TEST(FallbackNLPBackend, Normalise_LowercasesAndStripsDiacritics)
{
    EXPECT_EQ(FallbackNLPBackend::normalise("Abrir"), "abrir");
    EXPECT_EQ(FallbackNLPBackend::normalise("HABITACI\xC3\x93N"), "habitacion");
    EXPECT_EQ(FallbackNLPBackend::normalise("ni\xC3\xB1o"), "nino");
}

TEST(FallbackNLPBackend, Stem_StripsKnownSpanishSuffix)
{
    // "amigos" -> "amig" via the trailing "os" rule; the root is long
    // enough to clear the minimum-root-length guard.
    EXPECT_EQ(FallbackNLPBackend::stem("amigos", "es_ES"), "amig");
}

TEST(FallbackNLPBackend, Stem_NeverErasesShortRoot)
{
    // "es" itself must not be reduced below the minimum root length.
    const std::string result = FallbackNLPBackend::stem("es", "es_ES");
    EXPECT_FALSE(result.empty());
}

TEST(FallbackNLPBackend, Stem_UnknownLanguagePassesThroughUnchanged)
{
    EXPECT_EQ(FallbackNLPBackend::stem("running", "ja_JP"), "running");
}

TEST(FallbackNLPBackend, Stem_LanguageCodeWithoutRegion_PassesThroughUnchanged)
{
    // Exercises the branch of the internal baseLang() helper that finds no
    // '_' separator and returns the code as-is (e.g. a bare "eo").
    EXPECT_EQ(FallbackNLPBackend::stem("running", "eo"), "running");
}
