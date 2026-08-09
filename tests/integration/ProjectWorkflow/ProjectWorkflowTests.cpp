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

TEST(ProjectWorkflow, BuildTwoRoomAdventure_DataThreadsThroughEntityLayer)
{
    Core::Project project("The Old Library");

    // Two connected scenes.
    Entities::Scene* library = project.addScene("scene-library", "Old Library");
    Entities::Scene* hallway = project.addScene("scene-hallway", "Dusty Hallway");
    ASSERT_NE(library, nullptr);
    ASSERT_NE(hallway, nullptr);

    library->setStartScene(true);
    library->setDescription("Shelves of forgotten books line the walls.");

    // Exits live on the DataObject only — Entities::Scene doesn't expose
    // an accessor for them — so wire the connection the way the
    // node-editor/serialisation layer eventually will: through
    // Project::getSceneData().
    for (const auto& sceneData : project.getSceneData()) {
        if (sceneData->getId() == library->getId()) {
            sceneData->setExits({Data::Exit{"north", hallway->getId()}});
        }
    }

    // A player character starting in the library, carrying an item.
    Entities::Character* hero = project.addCharacter("char-hero", "Adventurer");
    ASSERT_NE(hero, nullptr);
    hero->setPlayer(true);
    hero->setStartingSceneId(library->getId());
    hero->setHealth(80);

    // An item placed in the hallway.
    Entities::Item* key = project.addItem("item-key", "Rusty Key");
    ASSERT_NE(key, nullptr);
    key->setStartingSceneId(hallway->getId());
    key->setItemType(1); // "Key" per Entities::Item::getItemTypes()

    // --- Verify the whole graph is consistent when read back ---

    EXPECT_TRUE(library->isStartScene());
    EXPECT_FALSE(hallway->isStartScene());
    EXPECT_EQ(library->getDescription(), "Shelves of forgotten books line the walls.");

    const Data::SceneData* libraryData = nullptr;
    for (const auto& sceneData : project.getSceneData()) {
        if (sceneData->getId() == library->getId()) {
            libraryData = sceneData.get();
        }
    }
    ASSERT_NE(libraryData, nullptr);
    ASSERT_EQ(libraryData->getExits().size(), 1u);
    EXPECT_EQ(libraryData->getExits()[0].direction, "north");
    EXPECT_EQ(libraryData->getExits()[0].targetSceneId, hallway->getId());

    Entities::Character* foundHero = project.findCharacter("char-hero");
    ASSERT_NE(foundHero, nullptr);
    EXPECT_TRUE(foundHero->isPlayer());
    EXPECT_EQ(foundHero->getStartingSceneId(), "scene-library");
    EXPECT_EQ(foundHero->getHealth(), 80);

    Entities::Item* foundKey = project.findItem("item-key");
    ASSERT_NE(foundKey, nullptr);
    EXPECT_EQ(foundKey->getStartingSceneId(), "scene-hallway");
    EXPECT_EQ(foundKey->getItemTypeName(), "Key");

    EXPECT_EQ(project.getScenes().size(), 2u);
    EXPECT_EQ(project.getCharacters().size(), 1u);
    EXPECT_EQ(project.getItems().size(), 1u);
}

TEST(ProjectWorkflow, InspectorPropertyRoundTrip_SetPropertyValueMatchesDirectSetter)
{
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene("scene-library", "Old Library");
    ASSERT_NE(library, nullptr);

    // Simulate the inspector panel: it only knows property IDs and
    // Inspector::PropertyValue, not the concrete Scene setter methods.
    bool accepted = library->setPropertyValue("width", 1024);
    ASSERT_TRUE(accepted);
    accepted = library->setPropertyValue("isStartScene", true);
    ASSERT_TRUE(accepted);

    // Read back the same way the inspector would, and cross-check against
    // the concrete accessor.
    auto widthValue = library->getPropertyValue("width");
    ASSERT_TRUE(std::holds_alternative<int>(widthValue));
    EXPECT_EQ(std::get<int>(widthValue), library->getWidth());
    EXPECT_EQ(library->getWidth(), 1024);
    EXPECT_TRUE(library->isStartScene());
}

TEST(ProjectWorkflow, RemovingScene_LeavesCharacterStartingSceneIdDangling)
{
    // Project doesn't cascade-delete references between entity types (no
    // referential integrity across Scene/Character/Item) — removing a
    // scene that a character points to via startingSceneId is legal and
    // the dangling string id is left as-is. This documents that current
    // behavior so a future cascade-delete feature has a regression test
    // to update deliberately, rather than an accidental behavior change.
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene("scene-library", "Old Library");
    Entities::Character* hero = project.addCharacter("char-hero", "Adventurer");
    hero->setStartingSceneId(library->getId());

    project.removeScene("scene-library");

    EXPECT_EQ(project.findScene("scene-library"), nullptr);
    EXPECT_EQ(hero->getStartingSceneId(), "scene-library");
    EXPECT_EQ(project.findCharacter("char-hero"), hero);
}

TEST(ProjectWorkflow, PropertyChangeEvents_FireAcrossEntityTypesIndependently)
{
    Core::Project project("The Old Library");
    Entities::Scene* library = project.addScene("scene-library", "Old Library");
    Entities::Character* hero = project.addCharacter("char-hero", "Adventurer");

    int sceneEvents = 0;
    int characterEvents = 0;
    library->getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { ++sceneEvents; });
    hero->getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { ++characterEvents; });

    library->setWidth(1024);
    hero->setHealth(50);
    hero->setHealth(50); // unchanged — must not fire again

    EXPECT_EQ(sceneEvents, 1);
    EXPECT_EQ(characterEvents, 1);
}

TEST(ProjectWorkflow, MultipleScenesCharactersItems_IndependentLifecycles)
{
    Core::Project project("The Old Library");

    for (int i = 0; i < 5; ++i) {
        std::string id = "scene-" + std::to_string(i);
        ASSERT_NE(project.addScene(id, "Scene " + std::to_string(i)), nullptr);
    }
    for (int i = 0; i < 3; ++i) {
        std::string id = "char-" + std::to_string(i);
        ASSERT_NE(project.addCharacter(id, "Character " + std::to_string(i)), nullptr);
    }

    project.removeScene("scene-2");
    project.removeCharacter("char-1");

    EXPECT_EQ(project.getScenes().size(), 4u);
    EXPECT_EQ(project.getCharacters().size(), 2u);
    EXPECT_EQ(project.findScene("scene-2"), nullptr);
    EXPECT_EQ(project.findCharacter("char-1"), nullptr);
    // Survivors are still reachable and independent.
    EXPECT_NE(project.findScene("scene-0"), nullptr);
    EXPECT_NE(project.findScene("scene-4"), nullptr);
    EXPECT_NE(project.findCharacter("char-0"), nullptr);
    EXPECT_NE(project.findCharacter("char-2"), nullptr);
}
