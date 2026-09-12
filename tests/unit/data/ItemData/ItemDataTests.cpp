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
using ADS::Types::ObjectId;
using ADS::Types::SceneId;
using ADS::Types::StateId;

TEST(ItemData, DefaultConstruction_HasExpectedDefaults)
{
    ItemData item;

    EXPECT_TRUE(item.getDescriptionTexts().normal.empty());
    EXPECT_EQ(item.getItemType(), 0);
    EXPECT_TRUE(item.getAffordances().empty());
    EXPECT_FALSE(item.isContainer());
    EXPECT_EQ(item.getWeight(), 0);
    EXPECT_EQ(item.getSlots(), 1);
    EXPECT_EQ(item.getServiceLife(), 0);
    EXPECT_TRUE(item.getImagePath().empty());
    EXPECT_TRUE(item.getContainerItemIds().empty());
    EXPECT_TRUE(item.getCombinableWithIds().empty());
    EXPECT_TRUE(item.getSynonyms().empty());
    EXPECT_TRUE(item.getAbbreviatures().empty());
    EXPECT_EQ(item.getDamageEffect(), Effect{});
    EXPECT_EQ(item.getHealEffect(), Effect{});
    EXPECT_FALSE(item.getInitialSceneId().has_value());
    EXPECT_FALSE(item.getState().has_value());
}

TEST(ItemData, SetDescriptionTexts_UpdatesNormalSlot)
{
    ItemData item;

    DescriptionTexts texts;
    texts.normal = {{"es_ES", "Una llave oxidada."}, {"en_US", "A rusty old key."}};
    item.setDescriptionTexts(texts);

    LocalizedText expected = {{"es_ES", "Una llave oxidada."}, {"en_US", "A rusty old key."}};
    EXPECT_EQ(item.getDescriptionTexts().normal, expected);
}

TEST(ItemData, SetItemType_UpdatesType)
{
    ItemData item;

    item.setItemType(2);

    EXPECT_EQ(item.getItemType(), 2);
}

TEST(ItemData, SetAffordances_UpdatesList)
{
    ItemData item;

    item.setAffordances({{"Takeable", {"on_pickup", "on_drop"}}, {"Openable", {"on_open"}}});

    ASSERT_EQ(item.getAffordances().size(), 2u);
    EXPECT_EQ(item.getAffordances()[0].name, "Takeable");
    EXPECT_EQ(item.getAffordances()[0].triggers, (std::vector<std::string>{"on_pickup", "on_drop"}));
    EXPECT_EQ(item.getAffordances()[1].name, "Openable");
}

TEST(ItemData, SetContainer_RoundTrips)
{
    ItemData item;

    EXPECT_FALSE(item.isContainer());

    item.setContainer(true);
    EXPECT_TRUE(item.isContainer());

    item.setContainer(false);
    EXPECT_FALSE(item.isContainer());
}

TEST(ItemData, SetWeight_UpdatesWeight)
{
    ItemData item;

    item.setWeight(200);

    EXPECT_EQ(item.getWeight(), 200);
}

TEST(ItemData, SetSlots_UpdatesSlots)
{
    ItemData item;

    item.setSlots(3);

    EXPECT_EQ(item.getSlots(), 3);
}

TEST(ItemData, SetServiceLife_UpdatesServiceLife)
{
    ItemData item;

    item.setServiceLife(50);

    EXPECT_EQ(item.getServiceLife(), 50);
}

TEST(ItemData, SetImagePath_UpdatesPath)
{
    ItemData item;

    item.setImagePath("assets/key.png");

    EXPECT_EQ(item.getImagePath(), "assets/key.png");
}

TEST(ItemData, SetContainerAndCombinableIds_UpdateIndependently)
{
    ItemData item;

    item.setContainerItemIds({ObjectId(4), ObjectId(5)});
    item.setCombinableWithIds({ObjectId(6)});

    ASSERT_EQ(item.getContainerItemIds().size(), 2u);
    EXPECT_EQ(item.getContainerItemIds()[0], ObjectId(4));
    ASSERT_EQ(item.getCombinableWithIds().size(), 1u);
    EXPECT_EQ(item.getCombinableWithIds()[0], ObjectId(6));
}

TEST(ItemData, SetSynonymsAndAbbreviatures_RoundTrip)
{
    ItemData item;

    item.setSynonyms({"lantern", "lamp"});
    item.setAbbreviatures({"lant"});

    EXPECT_EQ(item.getSynonyms(), (std::vector<std::string>{"lantern", "lamp"}));
    EXPECT_EQ(item.getAbbreviatures(), (std::vector<std::string>{"lant"}));
}

TEST(ItemData, SetEffects_RoundTrip)
{
    ItemData item;

    Effect dmg{1, 20, 83, 15, 2, 1};
    item.setDamageEffect(dmg);

    EXPECT_EQ(item.getDamageEffect(), dmg);
    EXPECT_EQ(item.getHealEffect(), Effect{});
}

TEST(ItemData, SetInitialSceneId_UpdatesId)
{
    ItemData item;

    item.setInitialSceneId(SceneId(1));

    EXPECT_EQ(item.getInitialSceneId(), SceneId(1));
}

TEST(ItemData, SetState_UpdatesState)
{
    ItemData item;

    item.setState(StateId(2));

    EXPECT_EQ(item.getState(), StateId(2));
}
