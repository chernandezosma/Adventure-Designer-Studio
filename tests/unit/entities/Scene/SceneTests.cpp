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
using ADS::Types::SceneId;

namespace {
    Data::SceneData makeSceneData()
    {
        Data::SceneData data;
        data.setId(SceneId(1));
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

TEST(Scene, GetSceneId_ReturnsTypedId)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    EXPECT_EQ(scene.getSceneId(), SceneId(1));
}

TEST(Scene, GetId_ReturnsStringifiedId)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    EXPECT_EQ(scene.getId(), "1");
}

TEST(Scene, GetPropertyDescriptors_ReturnsTwentyFourDescriptors)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto descriptors = scene.getPropertyDescriptors();

    // id(1) + General(name/isStartScene/state=3) + affordances(1) +
    // image(1) + descriptions(4) + presentItemIds(1) + exits(10) +
    // triggers(3: on_enter/on_exit/on_examine — on_turn is Game-only and
    // on_item_taken/dropped/used moved to Item's own trigger map) = 24
    EXPECT_EQ(descriptors.size(), 24u);
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

TEST(Scene, GetPropertyDescriptors_StateDescriptorAllowsCreateNew)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto descriptors = scene.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
                            [](const auto& d) { return d.getId() == "state"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_TRUE(it->isAllowCreateNew());
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

TEST(Scene, GetPropertyValue_Image_ReturnsString)
{
    Data::SceneData data = makeSceneData();
    data.setImage("assets/library.png");
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("image");

    ASSERT_TRUE(std::holds_alternative<std::string>(value));
    EXPECT_EQ(std::get<std::string>(value), "assets/library.png");
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

TEST(Scene, SetPropertyValue_UnknownId_ReturnsFalse)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    EXPECT_FALSE(scene.setPropertyValue("no-such-property", std::string("x")));
}

TEST(Scene, SetStartScene_FiresChangeEvent)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    bool fired = false;
    scene.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "isStartScene");
    });

    scene.setStartScene(true);

    EXPECT_TRUE(fired);
    EXPECT_TRUE(scene.isStartScene());
}

TEST(Scene, SetStartScene_UpdatesUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setStartScene(true);

    EXPECT_TRUE(scene.isStartScene());
}

TEST(Scene, SetImage_UpdatesUnderlyingData)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setImage("assets/library.png");

    EXPECT_EQ(scene.getImage(), "assets/library.png");
}

TEST(Scene, DescriptionsExitsItemsTriggers_AccessibleDirectlyAndThroughProperties)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    Data::Descriptions desc{.normal = 1, .longText = 2};
    scene.setDescriptions(desc);
    EXPECT_EQ(scene.getDescriptions(), desc);

    Data::SceneData::Exits exits;
    exits.north = SceneId(2);
    scene.setExits(exits);
    EXPECT_EQ(scene.getExits(), exits);

    std::vector<ADS::Types::ObjectId> items = {ADS::Types::ObjectId(1)};
    scene.setPresentItemIds(items);
    EXPECT_EQ(scene.getPresentItemIds(), items);

    scene.addTrigger(0x01, ADS::Types::EventId(5));
    EXPECT_EQ(scene.getTriggers().at(0x01), std::vector<ADS::Types::EventId>{ADS::Types::EventId(5)});

    // These fields are reachable both directly and through properties.
    auto descriptors = scene.getPropertyDescriptors();
    auto hasId = [&descriptors](const std::string& id) {
        return std::any_of(descriptors.begin(), descriptors.end(),
            [&id](const auto& d) { return d.getId() == id; });
    };
    EXPECT_TRUE(hasId("descriptionsNormal"));
    EXPECT_TRUE(hasId("descriptionsLongText"));
    EXPECT_TRUE(hasId("exitNorth"));
    EXPECT_TRUE(hasId("presentItemIds"));
    EXPECT_TRUE(hasId("triggerOnEnter"));
}

TEST(Scene, GetPropertyValue_DescriptionTexts_ReturnsLocalizedText)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    Data::DescriptionTexts texts{
        .normal = {{"es_ES", "Una habitación polvorienta."}, {"en_US", "A dusty room."}},
        .longText = {{"en_US", "Shelves line the walls."}},
    };
    scene.setDescriptionTexts(texts);

    auto normal = scene.getPropertyValue("descriptionsNormal");
    ASSERT_TRUE(std::holds_alternative<Inspector::LocalizedText>(normal));
    EXPECT_EQ(std::get<Inspector::LocalizedText>(normal), texts.normal);

    Inspector::LocalizedText newLongText = {{"en_US", "New long text."}};
    bool accepted = scene.setPropertyValue("descriptionsLongText", newLongText);
    EXPECT_TRUE(accepted);
    EXPECT_EQ(scene.getDescriptionTexts().longText, newLongText);
}

TEST(Scene, GetPropertyValue_State_NoProject_ReturnsEmptySelection)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("state");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}

TEST(Scene, SetPropertyValue_State_NoProject_ClearsState)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    bool accepted = scene.setPropertyValue("state", Inspector::SelectValue({0}, {}));

    EXPECT_TRUE(accepted);
    EXPECT_FALSE(data.getState().has_value());
}

TEST(Scene, GetSetState_TypedAccessor_RoundTrips)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    scene.setState(ADS::Types::StateId(4));

    ASSERT_TRUE(scene.getState().has_value());
    EXPECT_EQ(scene.getState(), ADS::Types::StateId(4));
    EXPECT_EQ(data.getState(), ADS::Types::StateId(4));
}

TEST(Scene, GetPropertyValue_Exit_NoProject_ReturnsEmptySelection)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("exitNorth");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}

TEST(Scene, GetPropertyValue_PresentItemIds_NoProject_ReturnsEmptySelection)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);
    scene.setPresentItemIds({ADS::Types::ObjectId(1)});

    auto value = scene.getPropertyValue("presentItemIds");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    // Without a Project back-pointer, the id can't be resolved to an
    // option index — this documents that limitation rather than crashing.
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
}

TEST(Scene, GetPropertyValue_Trigger_AlwaysEmptyOptions)
{
    Data::SceneData data = makeSceneData();
    Entities::Scene scene(&data);

    auto value = scene.getPropertyValue("triggerOnEnter");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}