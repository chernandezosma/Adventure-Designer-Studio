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

#include "Lexicon/LexEntry.h"

using namespace ADS;
using namespace ADS::Lexicon;

TEST(LexEntry, DefaultConstruction_HasNoTypesAndIsNotIndexed)
{
    LexEntry entry;

    EXPECT_EQ(entry.types(), WordTypeBits::None);
    EXPECT_EQ(entry.dominantType(), WordTypeBits::None);
    EXPECT_FALSE(entry.isAmbiguous());
    EXPECT_FALSE(entry.isIndexed());
    EXPECT_EQ(entry.compiledToken(), Token::UNASSIGNED);
}

TEST(LexEntry, ObserveType_SingleCategory_SetsDominantType)
{
    LexEntry entry;

    entry.observeType(WordTypeBits::Verb);

    EXPECT_EQ(entry.types(), WordTypeBits::Verb);
    EXPECT_EQ(entry.dominantType(), WordTypeBits::Verb);
    EXPECT_EQ(entry.typeCount(WordTypeBits::Verb), 1u);
    EXPECT_FALSE(entry.isAmbiguous());
}

TEST(LexEntry, ObserveType_MultipleCategories_TracksAmbiguityAndDominant)
{
    // Mirrors the documented "abierto" example: 3x Adjective, 1x Verb.
    LexEntry entry;

    entry.observeType(WordTypeBits::Adjective);
    entry.observeType(WordTypeBits::Verb);
    entry.observeType(WordTypeBits::Adjective);
    entry.observeType(WordTypeBits::Adjective);

    EXPECT_EQ(entry.types(), WordTypeBits::Verb | WordTypeBits::Adjective);
    EXPECT_TRUE(entry.isAmbiguous());
    EXPECT_EQ(entry.dominantType(), WordTypeBits::Adjective);
    EXPECT_EQ(entry.typeCount(WordTypeBits::Adjective), 3u);
    EXPECT_EQ(entry.typeCount(WordTypeBits::Verb), 1u);
}

TEST(LexEntry, ObserveType_DominantTypeSwitchesWhenCountOvertakes)
{
    LexEntry entry;

    entry.observeType(WordTypeBits::Noun);
    EXPECT_EQ(entry.dominantType(), WordTypeBits::Noun);

    entry.observeType(WordTypeBits::Verb);
    entry.observeType(WordTypeBits::Verb);
    EXPECT_EQ(entry.dominantType(), WordTypeBits::Verb);
}

TEST(LexEntry, Accumulate_ComputesNormalisedFrequency)
{
    LexEntry entry;

    entry.accumulate(4); // 1 occurrence out of 4 total corpus tokens
    EXPECT_EQ(entry.rawCount(), 1u);
    EXPECT_FLOAT_EQ(entry.frequency(), 0.25f);

    entry.accumulate(8); // 2 occurrences out of 8 total corpus tokens
    EXPECT_EQ(entry.rawCount(), 2u);
    EXPECT_FLOAT_EQ(entry.frequency(), 0.25f);
}

TEST(LexEntry, Accumulate_ZeroTotalTokens_YieldsZeroFrequency)
{
    LexEntry entry;

    entry.accumulate(0);

    EXPECT_EQ(entry.rawCount(), 1u);
    EXPECT_FLOAT_EQ(entry.frequency(), 0.0f);
}

TEST(LexEntry, CompiledToken_UnassignedUntilSet)
{
    LexEntry entry;

    EXPECT_FALSE(entry.isIndexed());
    EXPECT_EQ(entry.encodedSize(), 0u);

    entry.setCompiledToken(Token::SINGLE_BYTE_MIN);
    EXPECT_TRUE(entry.isIndexed());
    EXPECT_EQ(entry.encodedSize(), 1u);

    entry.setCompiledToken(Token::EXTENDED_MIN);
    EXPECT_EQ(entry.encodedSize(), 3u);
}

TEST(LexEntry, FindSynonym_ReturnsMatchingLinkOrNull)
{
    LexEntry entry;
    entry.synonyms.push_back(SynonymLink{.target = 7, .confidence = 0.9f, .confirmed = false});

    const SynonymLink* found = entry.findSynonym(7);
    ASSERT_NE(found, nullptr);
    EXPECT_FLOAT_EQ(found->confidence, 0.9f);

    EXPECT_EQ(entry.findSynonym(99), nullptr);
}

TEST(LexEntry, Ordering_DescendingFrequencyThenAlphabetical)
{
    LexEntry high;
    high.canonical = "zeta";
    high.accumulate(10); // frequency 0.1

    LexEntry low;
    low.canonical = "alpha";
    low.accumulate(100); // frequency 0.01

    EXPECT_LT(high, low); // higher frequency sorts first ("less than" in ranking order)

    LexEntry tieA;
    tieA.canonical = "alpha";
    tieA.accumulate(10);

    LexEntry tieB;
    tieB.canonical = "beta";
    tieB.accumulate(10);

    EXPECT_LT(tieA, tieB); // equal frequency: alphabetical tiebreak
}

TEST(LexEntry, RestoreTypesAndCounts_UsedByDeserialization)
{
    LexEntry entry;

    entry.restoreTypes(WordTypeBits::Noun | WordTypeBits::Adjective);
    entry.restoreCounts(42, 0.12f);

    EXPECT_EQ(entry.types(), WordTypeBits::Noun | WordTypeBits::Adjective);
    EXPECT_EQ(entry.dominantType(), WordTypeBits::None); // not reconstructed, per documented limitation
    EXPECT_EQ(entry.rawCount(), 42u);
    EXPECT_FLOAT_EQ(entry.frequency(), 0.12f);
}
