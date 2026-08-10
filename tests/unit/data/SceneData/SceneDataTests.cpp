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

TEST(Exit, Equality_SameValues_ReturnsTrue)
{
    Exit a{"north", "scene-02"};
    Exit b{"north", "scene-02"};

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(Exit, Inequality_DifferentDirection_ReturnsTrue)
{
    Exit a{"north", "scene-02"};
    Exit b{"south", "scene-02"};

    EXPECT_TRUE(a != b);
}

TEST(Exit, Inequality_DifferentTargetSceneId_ReturnsTrue)
{
    Exit a{"north", "scene-02"};
    Exit b{"north", "scene-03"};

    EXPECT_TRUE(a != b);
}

TEST(SceneData, DefaultConstruction_HasExpectedDefaults)
{
    SceneData scene;

    EXPECT_TRUE(scene.getDescription().empty());
    EXPECT_FALSE(scene.isStartScene());
    EXPECT_EQ(scene.getBackgroundColor(), ADS::Types::Color(0.2f, 0.2f, 0.2f, 1.0f));
    EXPECT_EQ(scene.getWidth(), 800);
    EXPECT_EQ(scene.getHeight(), 600);
    EXPECT_TRUE(scene.getBackgroundImagePath().empty());
    EXPECT_TRUE(scene.getMusicPath().empty());
    EXPECT_TRUE(scene.getExits().empty());
    EXPECT_TRUE(scene.getPresentItemIds().empty());
    EXPECT_TRUE(scene.getPresentCharacterIds().empty());
}

TEST(SceneData, SetDescription_UpdatesDescription)
{
    SceneData scene;

    scene.setDescription("A dusty old library.");

    EXPECT_EQ(scene.getDescription(), "A dusty old library.");
}

TEST(SceneData, SetStartScene_UpdatesFlag)
{
    SceneData scene;

    scene.setStartScene(true);

    EXPECT_TRUE(scene.isStartScene());
}

TEST(SceneData, SetBackgroundColor_UpdatesColor)
{
    SceneData scene;
    ADS::Types::Color newColor(0.9f, 0.1f, 0.1f, 1.0f);

    scene.setBackgroundColor(newColor);

    EXPECT_EQ(scene.getBackgroundColor(), newColor);
}

TEST(SceneData, SetWidth_UpdatesWidth)
{
    SceneData scene;

    scene.setWidth(1024);

    EXPECT_EQ(scene.getWidth(), 1024);
}

TEST(SceneData, SetHeight_UpdatesHeight)
{
    SceneData scene;

    scene.setHeight(768);

    EXPECT_EQ(scene.getHeight(), 768);
}

TEST(SceneData, SetBackgroundImagePath_UpdatesPath)
{
    SceneData scene;

    scene.setBackgroundImagePath("assets/library.png");

    EXPECT_EQ(scene.getBackgroundImagePath(), "assets/library.png");
}

TEST(SceneData, SetMusicPath_UpdatesPath)
{
    SceneData scene;

    scene.setMusicPath("assets/library.ogg");

    EXPECT_EQ(scene.getMusicPath(), "assets/library.ogg");
}

TEST(SceneData, SetExits_RoundTripsExitVector)
{
    SceneData scene;
    std::vector<Exit> exits = {
        {"north", "scene-02"}, // LCOV_EXCL_LINE — gcov misattributes this line's hit count to the closing brace
        {"south", "scene-03"},
    };

    scene.setExits(exits);

    EXPECT_EQ(scene.getExits(), exits);
}

TEST(SceneData, SetPresentItemIds_RoundTripsVector)
{
    SceneData scene;
    std::vector<std::string> itemIds = {"item-01", "item-02"};

    scene.setPresentItemIds(itemIds);

    EXPECT_EQ(scene.getPresentItemIds(), itemIds);
}

TEST(SceneData, SetPresentCharacterIds_RoundTripsVector)
{
    SceneData scene;
    std::vector<std::string> characterIds = {"char-01"};

    scene.setPresentCharacterIds(characterIds);

    EXPECT_EQ(scene.getPresentCharacterIds(), characterIds);
}

TEST(SceneData, SetFakeProperty_UpdatesValue)
{
    SceneData scene;

    scene.setFakeProperty(true);
    EXPECT_TRUE(scene.getFakeProperty());

    scene.setFakeProperty(false);
    EXPECT_FALSE(scene.getFakeProperty());
}