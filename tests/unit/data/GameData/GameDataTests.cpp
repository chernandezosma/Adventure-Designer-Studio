/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#include "Data/GameData.h"

using namespace ADS::Data;

TEST(GameData, DefaultConstruction_HasExpectedDefaults)
{
    GameData game;

    EXPECT_EQ(game.getInventoryCapacity(), 20);
    EXPECT_TRUE(game.getTitle().empty());
    EXPECT_TRUE(game.getSynopsis().empty());
    EXPECT_TRUE(game.getAuthor().name.empty());
    EXPECT_TRUE(game.getAuthor().email.empty());
    EXPECT_EQ(game.getVersion(), "0.1.0");
    EXPECT_EQ(game.getLanguages().defaultId, 21);
    EXPECT_EQ(game.getLanguages().supportedIds, (std::vector<std::uint8_t>{21}));
}

TEST(GameData, SetInventoryCapacity_ClampsBelowOneToOne)
{
    GameData game;

    game.setInventoryCapacity(0);
    EXPECT_EQ(game.getInventoryCapacity(), 1);

    game.setInventoryCapacity(200);
    EXPECT_EQ(game.getInventoryCapacity(), 200);
}

TEST(GameData, Setters_RoundTrip)
{
    GameData game;

    game.setTitle("The Forgotten Crypt");
    game.setSynopsis("A lone adventurer descends.");
    game.setAuthor({"Ada Lovelace", "ada@example.com"});
    game.setVersion("1.2.3");
    game.setLanguages({7, {7, 21, 27}});

    EXPECT_EQ(game.getTitle(), "The Forgotten Crypt");
    EXPECT_EQ(game.getSynopsis(), "A lone adventurer descends.");
    EXPECT_EQ(game.getAuthor().name, "Ada Lovelace");
    EXPECT_EQ(game.getAuthor().email, "ada@example.com");
    EXPECT_EQ(game.getVersion(), "1.2.3");
    EXPECT_EQ(game.getLanguages().defaultId, 7);
    EXPECT_EQ(game.getLanguages().supportedIds, (std::vector<std::uint8_t>{7, 21, 27}));
}

TEST(GameData, Equality_EqualObjectsCompareEqual)
{
    GameData a;
    GameData b;
    EXPECT_TRUE(a == b);

    a.setTitle("X");
    a.setAuthor({"N", "e@e"});
    a.setVersion("9.9.9");
    a.setLanguages({3, {3, 4}});

    b.setTitle("X");
    b.setAuthor({"N", "e@e"});
    b.setVersion("9.9.9");
    b.setLanguages({3, {3, 4}});

    EXPECT_TRUE(a == b);
}

TEST(GameData, Equality_DiffersOnEachMember)
{
    const GameData base;

    {
        GameData g = base;
        g.setInventoryCapacity(30);
        EXPECT_FALSE(g == base);
    }
    {
        GameData g = base;
        g.setTitle("different");
        EXPECT_FALSE(g == base);
    }
    {
        GameData g = base;
        g.setSynopsis("different");
        EXPECT_FALSE(g == base);
    }
    {
        GameData g = base;
        g.setAuthor({"", "someone@example.com"}); // nested GameAuthor.email differs
        EXPECT_FALSE(g == base);
    }
    {
        GameData g = base;
        g.setVersion("2.0.0");
        EXPECT_FALSE(g == base);
    }
    {
        GameData g = base;
        g.setLanguages({21, {21, 7}}); // nested GameLanguages.supportedIds differs
        EXPECT_FALSE(g == base);
    }
}

TEST(GameLanguages, Equality_DiffersOnDefaultAndSupported)
{
    const GameLanguages base;

    GameLanguages differentDefault = base;
    differentDefault.defaultId = 7;
    EXPECT_FALSE(differentDefault == base);

    GameLanguages differentSupported = base;
    differentSupported.supportedIds = {21, 7};
    EXPECT_FALSE(differentSupported == base);
}
