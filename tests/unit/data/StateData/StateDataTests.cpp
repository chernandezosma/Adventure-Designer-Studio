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

#include "Data/StateData.h"

using namespace ADS::Data;
using ADS::Types::StateId;

TEST(StateData, DefaultConstruction_HasExpectedDefaults)
{
    StateData state;

    EXPECT_EQ(state.getDescriptions(), Descriptions{});
    EXPECT_EQ(state.getDescriptionTexts(), DescriptionTexts{});
    EXPECT_FALSE(state.getNext().has_value());
}

TEST(StateData, SetDescriptions_UpdatesDescriptions)
{
    StateData state;
    Descriptions desc{.normal = 1, .longText = 2, .odor = 3, .sound = std::nullopt};

    state.setDescriptions(desc);

    EXPECT_EQ(state.getDescriptions(), desc);
}

TEST(StateData, SetDescriptionTexts_UpdatesDescriptionTexts)
{
    StateData state;
    DescriptionTexts texts;
    texts.normal["en_US"] = "Locked";

    state.setDescriptionTexts(texts);

    EXPECT_EQ(state.getDescriptionTexts(), texts);
}

TEST(StateData, SetNext_RoundTripsOptionalStateId)
{
    StateData state;

    state.setNext(StateId(5));

    ASSERT_TRUE(state.getNext().has_value());
    EXPECT_EQ(state.getNext(), StateId(5));
}

TEST(StateData, SetNext_Nullopt_ClearsNext)
{
    StateData state;
    state.setNext(StateId(5));

    state.setNext(std::nullopt);

    EXPECT_FALSE(state.getNext().has_value());
}
