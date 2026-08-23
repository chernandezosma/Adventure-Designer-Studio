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

/**
 * @file ProjectWorkflowTests.cpp
 * @brief Integration tests exercising Core::Project, Data, and Entities together
 *
 * Unlike the unit tests under tests/unit/, which isolate a single class,
 * these tests build a small but realistic adventure (scenes connected by
 * exits, a character with a starting scene and inventory, an item placed
 * in the world) through the full Project -> DataObject -> Entity chain,
 * the way IDE panels actually would.
 */

#include <gtest/gtest.h>

#include "Core/Project.h"

using namespace ADS;
using ADS::Types::CharacterId;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;

TEST(ProjectWorkflow, BuildTwoRoomAdventure_DataThreadsThroughEntityLayer)
{
    Core::Project project("The Old Library");

    // Two connected scenes.
    Entities::Scene* library = project.addScene(SceneId(1), "Old Library");
    Entities::Scene* hallway = project.addScene(SceneId(2), "Dusty Hallway");
    ASSERT_NE(library, nullptr);
    ASSERT_NE(hallway, nullptr);

    library->setStartScene(true);
    library->setDescriptions(Data::Descriptions{.normal = 100, .longText = 101});

    // Exits live on the DataObject only — Entities::Scene exposes them
    // through getExits()/setExits(), the way the node-editor/serialisation
    // layer eventually will: through Project::getSceneData().
    for (const auto& sceneData : project.getSceneData()) {
        if (sceneData->getId() == library->getSceneId()) {
            Data::SceneData::Exits exits;
            exits.north = hallway->getSceneId();
            sceneData->setExits(exits);
        }
    }

    // A player character starting in the library, carrying an item.
    Entities::Character* hero = project.addCharacter(CharacterId(1), "Adventurer");
    ASSERT_NE(hero, nullptr);
    hero->setPlayer(true);
    hero->setInitialSceneId(library->getSceneId());
    hero->setCapacities({0, 80, 0, 0});

    // An item placed in the hallway.
    Entities::Item* key = project.addItem(ObjectId(1), "Rusty Key");
    ASSERT_NE(key, nullptr);
    key->setInitialSceneId(hallway->getSceneId());
    key->setItemType(1); // "Key" per Entities::Item::getItemTypes()

    // --- Verify the whole graph is consistent when read back ---

    EXPECT_TRUE(library->isStartScene());
    EXPECT_FALSE(hallway->isStartScene());
    EXPECT_EQ(library->getDescriptions().normal, 100);
    EXPECT_EQ(library->getDescriptions().longText, 101);

    const Data::SceneData* libraryData = nullptr;
    for (const auto& sceneData : project.getSceneData()) {
        if (sceneData->getId() == library->getSceneId()) {
            libraryData = sceneData.get();
        }
    }
    ASSERT_NE(libraryData, nullptr);
    EXPECT_EQ(libraryData->getExits().north, hallway->getSceneId());
    EXPECT_FALSE(libraryData->getExits().south.has_value());

    Entities::Character* foundHero = project.findCharacter(CharacterId(1));
    ASSERT_NE(foundHero, nullptr);
    EXPECT_TRUE(foundHero->isPlayer());
    EXPECT_EQ(foundHero->getInitialSceneId(), library->getSceneId());
    EXPECT_EQ(foundHero->getCapacities().life, 80);

    Entities::Item* foundKey = project.findItem(ObjectId(1));
    ASSERT_NE(foundKey, nullptr);
    EXPECT_EQ(foundKey->getInitialSceneId(), hallway->getSceneId());
    EXPECT_EQ(foundKey->getItemTypeName(), "Key");

    EXPECT_EQ(project.getScenes().size(), 2u);
    EXPECT_EQ(project.getCharacters().size(), 1u);
    EXPECT_EQ(project.getItems().size(), 1u);
}

TEST(ProjectWorkflow, InspectorPropertyRoundTrip_SetPropertyValueMatchesDirectSetter)
{
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene(SceneId(1), "Old Library");
    ASSERT_NE(library, nullptr);

    // Simulate the inspector panel: it only knows property IDs and
    // Inspector::PropertyValue, not the concrete Scene setter methods.
    // "state" is a Select referencing the project's shared State catalog.
    Entities::State* locked = project.addState(ADS::Types::StateId(1), "Locked");
    ASSERT_NE(locked, nullptr);
    bool accepted = library->setPropertyValue("state", Inspector::SelectValue({0}, {}));
    ASSERT_TRUE(accepted);
    accepted = library->setPropertyValue("isStartScene", true);
    ASSERT_TRUE(accepted);

    // Read back the same way the inspector would, and cross-check against
    // the concrete accessor.
    auto stateValue = library->getPropertyValue("state");
    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(stateValue));
    EXPECT_EQ(std::get<Inspector::SelectValue>(stateValue).selectedIndices, (std::vector<int>{0}));
    EXPECT_EQ(library->getState(), locked->getStateId());
    EXPECT_TRUE(library->isStartScene());
}

TEST(ProjectWorkflow, RemovingScene_LeavesCharacterInitialSceneIdDangling)
{
    // Project doesn't cascade-delete references between entity types (no
    // referential integrity across Scene/Character/Item) — removing a
    // scene that a character points to via initialSceneId is legal and
    // the dangling id is left as-is. This documents that current behavior
    // so a future cascade-delete feature has a regression test to update
    // deliberately, rather than an accidental behavior change.
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene(SceneId(1), "Old Library");
    Entities::Character* hero = project.addCharacter(CharacterId(1), "Adventurer");
    hero->setInitialSceneId(library->getSceneId());

    project.removeScene(SceneId(1));

    EXPECT_EQ(project.findScene(SceneId(1)), nullptr);
    EXPECT_EQ(hero->getInitialSceneId(), SceneId(1));
    EXPECT_EQ(project.findCharacter(CharacterId(1)), hero);
}

TEST(ProjectWorkflow, PropertyChangeEvents_FireAcrossEntityTypesIndependently)
{
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene(SceneId(1), "Old Library");
    Entities::Character* hero = project.addCharacter(CharacterId(1), "Adventurer");

    int sceneEvents = 0;
    int characterEvents = 0;
    library->getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { ++sceneEvents; });
    hero->getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { ++characterEvents; });

    library->setStartScene(true);
    hero->setCapacities({0, 50, 0, 0});
    hero->setCapacities({0, 50, 0, 0}); // unchanged — must not fire again

    EXPECT_EQ(sceneEvents, 1);
    EXPECT_EQ(characterEvents, 1);
}

TEST(ProjectWorkflow, MultipleScenesCharactersItems_IndependentLifecycles)
{
    Core::Project project("The Old Library");

    for (int i = 0; i < 5; ++i) {
        SceneId id(static_cast<uint8_t>(i));
        ASSERT_NE(project.addScene(id, "Scene " + std::to_string(i)), nullptr);
    }
    for (int i = 0; i < 3; ++i) {
        CharacterId id(static_cast<uint8_t>(i));
        ASSERT_NE(project.addCharacter(id, "Character " + std::to_string(i)), nullptr);
    }

    project.removeScene(SceneId(2));
    project.removeCharacter(CharacterId(1));

    EXPECT_EQ(project.getScenes().size(), 4u);
    EXPECT_EQ(project.getCharacters().size(), 2u);
    EXPECT_EQ(project.findScene(SceneId(2)), nullptr);
    EXPECT_EQ(project.findCharacter(CharacterId(1)), nullptr);
    // Survivors are still reachable and independent.
    EXPECT_NE(project.findScene(SceneId(0)), nullptr);
    EXPECT_NE(project.findScene(SceneId(4)), nullptr);
    EXPECT_NE(project.findCharacter(CharacterId(0)), nullptr);
    EXPECT_NE(project.findCharacter(CharacterId(2)), nullptr);
}
