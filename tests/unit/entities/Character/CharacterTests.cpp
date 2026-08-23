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

#include "Data/CharacterData.h"
#include "Entities/Character.h"

using namespace ADS;
using ADS::Types::CharacterId;
using ADS::Types::SceneId;

namespace {
    Data::CharacterData makeCharacterData()
    {
        Data::CharacterData data;
        data.setId(CharacterId(1));
        data.setName("Hero");
        return data;
    }
}

TEST(Character, GetTypeName_ReturnsCharacter)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    EXPECT_EQ(character.getTypeName(), "Character");
}

TEST(Character, GetPropertyDescriptors_ReturnsTwentyOneDescriptors)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    // 17 base descriptors + 4 trigger keys (on_talk/on_die/on_heal/on_hurt)
    EXPECT_EQ(character.getPropertyDescriptors().size(), 21u);
}

TEST(Character, GetPropertyDescriptors_Capacities_RangeZeroTo255)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    auto descriptors = character.getPropertyDescriptors();
    auto life = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& d) { return d.getId() == "capLife"; });

    ASSERT_NE(life, descriptors.end());
    EXPECT_EQ(life->getConstraints().minValue, 0.0f);
    EXPECT_EQ(life->getConstraints().maxValue, 255.0f);
    EXPECT_EQ(life->getCategory(), "Capacities");
}

TEST(Character, GetPropertyValue_CapLife_ReturnsInt)
{
    Data::CharacterData data = makeCharacterData();
    data.setCapacities({0, 90, 0, 0});
    Entities::Character character(&data);

    auto value = character.getPropertyValue("capLife");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 90);
}

TEST(Character, GetPropertyValue_DialogColor_ReturnsImVec4)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    auto value = character.getPropertyValue("dialogColor");

    ASSERT_TRUE(std::holds_alternative<ImVec4>(value));
    ImVec4 color = std::get<ImVec4>(value);
    EXPECT_FLOAT_EQ(color.x, 1.0f);
    EXPECT_FLOAT_EQ(color.w, 1.0f);
}

TEST(Character, GetPropertyValue_UnknownId_ReturnsMonostate)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    EXPECT_TRUE(std::holds_alternative<std::monostate>(character.getPropertyValue("nope")));
}

TEST(Character, SetPropertyValue_IsPlayer_ValidType_Accepted)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    bool accepted = character.setPropertyValue("isPlayer", true);

    EXPECT_TRUE(accepted);
    EXPECT_TRUE(data.isPlayer());
}

TEST(Character, SetPropertyValue_CapLife_WrongType_Rejected)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    bool accepted = character.setPropertyValue("capLife", std::string("not an int"));

    EXPECT_FALSE(accepted);
    EXPECT_EQ(data.getCapacities().life, 0);
}

TEST(Character, SetPropertyValue_CapLife_UpdatesUnderlyingDataAndFiresEvent)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    bool fired = false;
    character.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "capacities");
    });

    bool accepted = character.setPropertyValue("capLife", 42);

    EXPECT_TRUE(accepted);
    EXPECT_TRUE(fired);
    EXPECT_EQ(character.getCapacities().life, 42);
}

TEST(Character, SetCapacities_SameValue_DoesNotFireEvent)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    bool fired = false;
    character.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { fired = true; });

    character.setCapacities(character.getCapacities());

    EXPECT_FALSE(fired);
}

TEST(Character, SetPlayer_UpdatesUnderlyingData)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    character.setPlayer(true);

    EXPECT_TRUE(character.isPlayer());
}

TEST(Character, SetDialogColor_ChangedValue_FiresEventWithImVec4Payload)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    Inspector::PropertyValue capturedNew;
    character.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        capturedNew = e.newValue;
    });

    character.setDialogColor(ADS::Types::Color(0.1f, 0.2f, 0.3f, 1.0f));

    ASSERT_TRUE(std::holds_alternative<ImVec4>(capturedNew));
    EXPECT_FLOAT_EQ(std::get<ImVec4>(capturedNew).x, 0.1f);
}

TEST(Character, SetImageAvatarAndInitialSceneId_UpdateUnderlyingData)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    character.setImagePath("assets/hero_full.png");
    character.setAvatarPath("assets/hero_face.png");
    character.setInitialSceneId(SceneId(1));

    EXPECT_EQ(character.getImagePath(), "assets/hero_full.png");
    EXPECT_EQ(character.getAvatarPath(), "assets/hero_face.png");
    EXPECT_EQ(character.getInitialSceneId(), SceneId(1));
}

TEST(Character, PropertyValue_DescriptionsNormal_RoundTripsLocalizedTextThroughInspectorContract)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    Inspector::LocalizedText texts = {{"es_ES", "Un aventurero curtido."}, {"en_US", "A grizzled adventurer."}};

    bool accepted = character.setPropertyValue("descriptionsNormal", texts);

    EXPECT_TRUE(accepted);
    auto value = character.getPropertyValue("descriptionsNormal");
    ASSERT_TRUE(std::holds_alternative<Inspector::LocalizedText>(value));
    EXPECT_EQ(std::get<Inspector::LocalizedText>(value), texts);
}
