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

#include "Data/CharacterData.h"

using namespace ADS::Data;
using ADS::Types::SceneId;
using ADS::Types::StateId;

TEST(CharacterData, DefaultConstruction_HasExpectedDefaults)
{
    CharacterData character;

    EXPECT_TRUE(character.getDescriptionTexts().normal.empty());
    EXPECT_FALSE(character.isPlayer());
    EXPECT_EQ(character.getCapacities(), Capacities{});
    EXPECT_FALSE(character.getState().has_value());
    EXPECT_EQ(character.getDialogColor(), ADS::Types::Color(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_TRUE(character.getImagePath().empty());
    EXPECT_TRUE(character.getAvatarPath().empty());
    EXPECT_FALSE(character.getInitialSceneId().has_value());
}

TEST(CharacterData, SetDescriptionTexts_UpdatesNormalSlot)
{
    CharacterData character;

    DescriptionTexts texts;
    texts.normal = {{"es_ES", "Un aventurero curtido."}, {"en_US", "A grizzled adventurer."}};
    character.setDescriptionTexts(texts);

    LocalizedText expected = {{"es_ES", "Un aventurero curtido."}, {"en_US", "A grizzled adventurer."}};
    EXPECT_EQ(character.getDescriptionTexts().normal, expected);
}

TEST(CharacterData, SetPlayer_UpdatesFlag)
{
    CharacterData character;

    character.setPlayer(true);

    EXPECT_TRUE(character.isPlayer());
}

TEST(CharacterData, SetCapacities_UpdatesEveryVitality)
{
    CharacterData character;

    character.setCapacities({10, 42, 30, 55});

    EXPECT_EQ(character.getCapacities().load, 10);
    EXPECT_EQ(character.getCapacities().life, 42);
    EXPECT_EQ(character.getCapacities().stamina, 30);
    EXPECT_EQ(character.getCapacities().sanity, 55);
}

TEST(CharacterData, SetState_UpdatesState)
{
    CharacterData character;

    character.setState(StateId(7));

    ASSERT_TRUE(character.getState().has_value());
    EXPECT_EQ(character.getState(), StateId(7));
}

TEST(CharacterData, SetDialogColor_UpdatesColor)
{
    CharacterData character;
    ADS::Types::Color newColor(0.1f, 0.2f, 0.3f, 1.0f);

    character.setDialogColor(newColor);

    EXPECT_EQ(character.getDialogColor(), newColor);
}

TEST(CharacterData, SetImageAndAvatarPaths_UpdateIndependently)
{
    CharacterData character;

    character.setImagePath("assets/hero_full.png");
    character.setAvatarPath("assets/hero_face.png");

    EXPECT_EQ(character.getImagePath(), "assets/hero_full.png");
    EXPECT_EQ(character.getAvatarPath(), "assets/hero_face.png");
}

TEST(CharacterData, SetInitialSceneId_UpdatesId)
{
    CharacterData character;

    character.setInitialSceneId(SceneId(1));

    EXPECT_EQ(character.getInitialSceneId(), SceneId(1));
}
