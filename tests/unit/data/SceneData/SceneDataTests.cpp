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

#include "Data/SceneData.h"

using namespace ADS::Data;
using ADS::Types::EventId;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;

TEST(SceneData, DefaultConstruction_HasExpectedDefaults)
{
    SceneData scene;

    EXPECT_EQ(scene.getDescriptions(), Descriptions{});
    EXPECT_TRUE(scene.getImage().empty());
    EXPECT_EQ(scene.getExits(), SceneData::Exits{});
    EXPECT_TRUE(scene.getPresentItemIds().empty());
    EXPECT_TRUE(scene.getTriggers().empty());
    EXPECT_FALSE(scene.isStartScene());
}

TEST(SceneData, SetDescriptions_UpdatesDescriptions)
{
    SceneData scene;
    Descriptions desc{.normal = 1, .longText = 2, .odor = 3, .sound = std::nullopt};

    scene.setDescriptions(desc);

    EXPECT_EQ(scene.getDescriptions(), desc);
}

TEST(SceneData, SetStartScene_UpdatesFlag)
{
    SceneData scene;

    scene.setStartScene(true);

    EXPECT_TRUE(scene.isStartScene());
}

TEST(SceneData, SetImage_UpdatesImage)
{
    SceneData scene;

    scene.setImage("assets/library.png");

    EXPECT_EQ(scene.getImage(), "assets/library.png");
}

TEST(SceneData, SetExits_RoundTripsExitsStruct)
{
    SceneData scene;
    SceneData::Exits exits;
    exits.north = SceneId(2);
    exits.up = SceneId(9);

    scene.setExits(exits);

    EXPECT_EQ(scene.getExits(), exits);
    EXPECT_EQ(scene.getExits().north, SceneId(2));
    EXPECT_EQ(scene.getExits().up, SceneId(9));
    EXPECT_FALSE(scene.getExits().south.has_value());
}

TEST(SceneData, SetPresentItemIds_RoundTripsVector)
{
    SceneData scene;
    std::vector<ObjectId> itemIds = {ObjectId(1), ObjectId(2)};

    scene.setPresentItemIds(itemIds);

    EXPECT_EQ(scene.getPresentItemIds(), itemIds);
}

TEST(SceneData, SetTriggers_ReplacesEntireMap)
{
    SceneData scene;
    std::map<uint8_t, std::vector<EventId>> triggers = {
        {0x01, {EventId(5)}},
    };

    scene.setTriggers(triggers);

    EXPECT_EQ(scene.getTriggers(), triggers);
}

TEST(SceneData, AddTrigger_NewEventId_CreatesEntry)
{
    SceneData scene;

    scene.addTrigger(0x01, EventId(5));

    ASSERT_EQ(scene.getTriggers().count(0x01), 1u);
    EXPECT_EQ(scene.getTriggers().at(0x01), std::vector<EventId>{EventId(5)});
}

TEST(SceneData, AddTrigger_ExistingEventId_AppendsToArray)
{
    SceneData scene;
    scene.addTrigger(0x01, EventId(5));

    scene.addTrigger(0x01, EventId(6));

    std::vector<EventId> expected = {EventId(5), EventId(6)};
    EXPECT_EQ(scene.getTriggers().at(0x01), expected);
}

TEST(SceneData, AddTrigger_DifferentEventIds_KeepsSeparateEntries)
{
    SceneData scene;

    scene.addTrigger(0x01, EventId(5));
    scene.addTrigger(0x02, EventId(6));

    EXPECT_EQ(scene.getTriggers().size(), 2u);
    EXPECT_EQ(scene.getTriggers().at(0x01), std::vector<EventId>{EventId(5)});
    EXPECT_EQ(scene.getTriggers().at(0x02), std::vector<EventId>{EventId(6)});
}
