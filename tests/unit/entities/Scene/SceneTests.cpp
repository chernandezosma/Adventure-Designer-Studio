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

#include <algorithm>

#include "Data/SceneData.h"
#include "Entities/Scene.h"

using namespace ADS;

namespace {
    Data::SceneData makeSceneData()
    {
        Data::SceneData data;
        data.setId("scene-01");
        data.setName("Old Library");
        return data;
    }
}

TEST(Scene, GetTypeName_ReturnsScene)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    EXPECT_EQ(scene.getTypeName(), "Scene");
}

TEST(Scene, GetPropertyDescriptors_ReturnsTenDescriptors)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto descriptors = scene.getPropertyDescriptors();

    EXPECT_EQ(descriptors.size(), 10u);
}

TEST(Scene, GetPropertyDescriptors_IdDescriptorIsReadOnly)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto descriptors = scene.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
                            [](const auto& d) { return d.getId() == "id"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_TRUE(it->isReadOnly());
}

TEST(Scene, GetPropertyValue_Name_ReturnsDataName)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("name");

    ASSERT_TRUE(std::holds_alternative<std::string>(value));
    EXPECT_EQ(std::get<std::string>(value), "Old Library");
}

TEST(Scene, GetPropertyValue_IsStartScene_ReturnsBool)
{
    Data::SceneData data = makeSceneData();
    data.setStartScene(true);
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("isStartScene");

    ASSERT_TRUE(std::holds_alternative<bool>(value));
    EXPECT_TRUE(std::get<bool>(value));
}

TEST(Scene, GetPropertyValue_Width_ReturnsInt)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("width");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 800);
}

TEST(Scene, GetPropertyValue_BackgroundColor_ReturnsImVec4)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("backgroundColor");

    ASSERT_TRUE(std::holds_alternative<ImVec4>(value));
    ImVec4 color = std::get<ImVec4>(value);
    EXPECT_FLOAT_EQ(color.x, 0.2f);
    EXPECT_FLOAT_EQ(color.y, 0.2f);
    EXPECT_FLOAT_EQ(color.z, 0.2f);
    EXPECT_FLOAT_EQ(color.w, 1.0f);
}

TEST(Scene, GetPropertyValue_UnknownId_ReturnsMonostate)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("no-such-property");

    EXPECT_TRUE(std::holds_alternative<std::monostate>(value));
}

TEST(Scene, SetPropertyValue_Name_ValidType_UpdatesAndReturnsTrue)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    bool accepted = scene.setPropertyValue("name", std::string("New Name"));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(data.getName(), "New Name");
}

TEST(Scene, SetPropertyValue_Width_WrongType_RejectedAndUnchanged)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    bool accepted = scene.setPropertyValue("width", std::string("not an int"));

    EXPECT_FALSE(accepted);
    EXPECT_EQ(data.getWidth(), 800);
}

TEST(Scene, SetPropertyValue_UnknownId_ReturnsFalse)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    EXPECT_FALSE(scene.setPropertyValue("no-such-property", std::string("x")));
}

TEST(Scene, SetPropertyValue_BackgroundColor_ConvertsImVec4ToColor)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    bool accepted = scene.setPropertyValue("backgroundColor", ImVec4(0.9f, 0.1f, 0.1f, 1.0f));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(data.getBackgroundColor(), ADS::Types::Color(0.9f, 0.1f, 0.1f, 1.0f));
}

TEST(Scene, SetFakeProperty_FiresChangeEvent)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    bool fired = false;
    scene.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "fakeProperty");
    });

    scene.setFakeProperty(true);

    EXPECT_TRUE(fired);
    EXPECT_TRUE(scene.getFakeProperty());
}

TEST(Scene, SetDescription_UpdatesUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setDescription("A dusty old library.");

    EXPECT_EQ(scene.getDescription(), "A dusty old library.");
    EXPECT_EQ(data.getDescription(), "A dusty old library.");
}

TEST(Scene, SetStartScene_UpdatesUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setStartScene(true);

    EXPECT_TRUE(scene.isStartScene());
}

TEST(Scene, SetBackgroundColor_ChangedValue_FiresEventWithImVec4Payload)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    Inspector::PropertyValue capturedOld, capturedNew;
    scene.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        capturedOld = e.oldValue;
        capturedNew = e.newValue;
    });

    scene.setBackgroundColor(ADS::Types::Color(1.0f, 0.0f, 0.0f, 1.0f));

    ASSERT_TRUE(std::holds_alternative<ImVec4>(capturedNew));
    EXPECT_FLOAT_EQ(std::get<ImVec4>(capturedNew).x, 1.0f);
    ASSERT_TRUE(std::holds_alternative<ImVec4>(capturedOld));
    EXPECT_FLOAT_EQ(std::get<ImVec4>(capturedOld).x, 0.2f);
}

TEST(Scene, SetBackgroundColor_SameValue_DoesNotFireEvent)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    bool fired = false;
    scene.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { fired = true; });

    scene.setBackgroundColor(scene.getBackgroundColor());

    EXPECT_FALSE(fired);
}

TEST(Scene, SetWidthAndHeight_UpdateUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setWidth(1024);
    scene.setHeight(768);

    EXPECT_EQ(scene.getWidth(), 1024);
    EXPECT_EQ(scene.getHeight(), 768);
}

TEST(Scene, SetBackgroundImagePathAndMusicPath_UpdateUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setBackgroundImagePath("assets/library.png");
    scene.setMusicPath("assets/library.ogg");

    EXPECT_EQ(scene.getBackgroundImagePath(), "assets/library.png");
    EXPECT_EQ(scene.getMusicPath(), "assets/library.ogg");
}
