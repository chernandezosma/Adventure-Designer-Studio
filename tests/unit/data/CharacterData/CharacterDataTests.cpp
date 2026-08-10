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

TEST(CharacterData, DefaultConstruction_HasExpectedDefaults)
{
    CharacterData character;

    EXPECT_TRUE(character.getDescription().empty());
    EXPECT_FALSE(character.isPlayer());
    EXPECT_EQ(character.getHealth(), 100);
    EXPECT_EQ(character.getMaxHealth(), 100);
    EXPECT_EQ(character.getDialogColor(), ADS::Types::Color(1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_TRUE(character.getPortraitPath().empty());
    EXPECT_TRUE(character.getStartingSceneId().empty());
    EXPECT_TRUE(character.getInventoryItemIds().empty());
}

TEST(CharacterData, SetDescription_UpdatesDescription)
{
    CharacterData character;

    character.setDescription("A grizzled adventurer.");

    EXPECT_EQ(character.getDescription(), "A grizzled adventurer.");
}

TEST(CharacterData, SetPlayer_UpdatesFlag)
{
    CharacterData character;

    character.setPlayer(true);

    EXPECT_TRUE(character.isPlayer());
}

TEST(CharacterData, SetHealth_UpdatesHealth)
{
    CharacterData character;

    character.setHealth(42);

    EXPECT_EQ(character.getHealth(), 42);
}

TEST(CharacterData, SetMaxHealth_UpdatesMaxHealth)
{
    CharacterData character;

    character.setMaxHealth(250);

    EXPECT_EQ(character.getMaxHealth(), 250);
}

TEST(CharacterData, SetDialogColor_UpdatesColor)
{
    CharacterData character;
    ADS::Types::Color newColor(0.1f, 0.2f, 0.3f, 1.0f);

    character.setDialogColor(newColor);

    EXPECT_EQ(character.getDialogColor(), newColor);
}

TEST(CharacterData, SetPortraitPath_UpdatesPath)
{
    CharacterData character;

    character.setPortraitPath("assets/hero.png");

    EXPECT_EQ(character.getPortraitPath(), "assets/hero.png");
}

TEST(CharacterData, SetStartingSceneId_UpdatesId)
{
    CharacterData character;

    character.setStartingSceneId("scene-01");

    EXPECT_EQ(character.getStartingSceneId(), "scene-01");
}

TEST(CharacterData, SetInventoryItemIds_RoundTripsVector)
{
    CharacterData character;
    std::vector<std::string> items = {"item-01", "item-02"};

    character.setInventoryItemIds(items);

    EXPECT_EQ(character.getInventoryItemIds(), items);
}