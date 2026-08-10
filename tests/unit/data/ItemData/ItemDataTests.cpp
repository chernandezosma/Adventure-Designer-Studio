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

#include "Data/ItemData.h"

using namespace ADS::Data;

TEST(ItemData, DefaultConstruction_HasExpectedDefaults)
{
    ItemData item;

    EXPECT_TRUE(item.getDescription().empty());
    EXPECT_EQ(item.getItemType(), 0);
    EXPECT_TRUE(item.isPickable());
    EXPECT_FALSE(item.isUsable());
    EXPECT_EQ(item.getQuantity(), 1);
    EXPECT_TRUE(item.getIconPath().empty());
    EXPECT_TRUE(item.getStartingSceneId().empty());
}

TEST(ItemData, SetDescription_UpdatesDescription)
{
    ItemData item;

    item.setDescription("A rusty old key.");

    EXPECT_EQ(item.getDescription(), "A rusty old key.");
}

TEST(ItemData, SetItemType_UpdatesType)
{
    ItemData item;

    item.setItemType(2);

    EXPECT_EQ(item.getItemType(), 2);
}

TEST(ItemData, SetPickable_UpdatesFlag)
{
    ItemData item;

    item.setPickable(false);

    EXPECT_FALSE(item.isPickable());
}

TEST(ItemData, SetUsable_UpdatesFlag)
{
    ItemData item;

    item.setUsable(true);

    EXPECT_TRUE(item.isUsable());
}

TEST(ItemData, SetQuantity_UpdatesQuantity)
{
    ItemData item;

    item.setQuantity(5);

    EXPECT_EQ(item.getQuantity(), 5);
}

TEST(ItemData, SetIconPath_UpdatesPath)
{
    ItemData item;

    item.setIconPath("assets/key.png");

    EXPECT_EQ(item.getIconPath(), "assets/key.png");
}

TEST(ItemData, SetStartingSceneId_UpdatesId)
{
    ItemData item;

    item.setStartingSceneId("scene-01");

    EXPECT_EQ(item.getStartingSceneId(), "scene-01");
}