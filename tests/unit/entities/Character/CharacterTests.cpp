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
#include "Entities/Character.h"

using namespace ADS;

namespace {
    Data::CharacterData makeCharacterData()
    {
        Data::CharacterData data;
        data.setId("char-01");
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

TEST(Character, GetPropertyDescriptors_ReturnsNineDescriptors)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    EXPECT_EQ(character.getPropertyDescriptors().size(), 9u);
}

TEST(Character, GetPropertyValue_Health_ReturnsInt)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    auto value = character.getPropertyValue("health");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 100);
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

TEST(Character, SetPropertyValue_Health_WrongType_Rejected)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    bool accepted = character.setPropertyValue("health", std::string("not an int"));

    EXPECT_FALSE(accepted);
    EXPECT_EQ(data.getHealth(), 100);
}

TEST(Character, SetHealth_ChangedValue_FiresEvent)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    bool fired = false;
    character.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "health");
    });

    character.setHealth(42);

    EXPECT_TRUE(fired);
    EXPECT_EQ(character.getHealth(), 42);
}

TEST(Character, SetMaxHealth_UpdatesUnderlyingData)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    character.setMaxHealth(250);

    EXPECT_EQ(character.getMaxHealth(), 250);
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

TEST(Character, SetDialogColor_SameValue_DoesNotFireEvent)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);
    bool fired = false;
    character.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { fired = true; });

    character.setDialogColor(character.getDialogColor());

    EXPECT_FALSE(fired);
}

TEST(Character, SetPortraitPathAndStartingSceneId_UpdateUnderlyingData)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    character.setPortraitPath("assets/hero.png");
    character.setStartingSceneId("scene-01");

    EXPECT_EQ(character.getPortraitPath(), "assets/hero.png");
    EXPECT_EQ(character.getStartingSceneId(), "scene-01");
}

TEST(Character, SetDescription_UpdatesUnderlyingData)
{
    Data::CharacterData data = makeCharacterData();
    Entities::Character character(&data);

    character.setDescription("A grizzled adventurer.");

    EXPECT_EQ(character.getDescription(), "A grizzled adventurer.");
}
