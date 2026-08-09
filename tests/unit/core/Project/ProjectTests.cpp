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

#include "Core/Project.h"

using namespace ADS;

TEST(Project, Construction_SetsName)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.getName(), "My Adventure");
}

TEST(Project, SetName_UpdatesName)
{
    Core::Project project("My Adventure");

    project.setName("Renamed Adventure");

    EXPECT_EQ(project.getName(), "Renamed Adventure");
}

TEST(Project, DefaultConstruction_IsNotSaved)
{
    Core::Project project("My Adventure");

    EXPECT_FALSE(project.isSaved());
}

TEST(Project, SetFilePath_MarksAsSavedAndStoresPath)
{
    Core::Project project("My Adventure");

    project.setFilePath("/tmp/my-adventure.ads");

    EXPECT_TRUE(project.isSaved());
    EXPECT_EQ(project.getFilePath(), std::filesystem::path("/tmp/my-adventure.ads"));
}

TEST(Project, ClearFilePath_MarksAsUnsaved)
{
    Core::Project project("My Adventure");
    project.setFilePath("/tmp/my-adventure.ads");

    project.clearFilePath();

    EXPECT_FALSE(project.isSaved());
}

// --- Scene CRUD ---

TEST(Project, AddScene_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::Scene* scene = project.addScene("scene-01", "Old Library");

    ASSERT_NE(scene, nullptr);
    EXPECT_EQ(scene->getId(), "scene-01");
    EXPECT_EQ(scene->getDisplayName(), "Old Library");
    EXPECT_EQ(project.getScenes().size(), 1u);
    EXPECT_EQ(project.getSceneData().size(), 1u);
}

TEST(Project, AddScene_DuplicateId_ReturnsNullptrAndDoesNotAdd)
{
    Core::Project project("My Adventure");
    project.addScene("scene-01", "Old Library");

    Entities::Scene* duplicate = project.addScene("scene-01", "Another Name");

    EXPECT_EQ(duplicate, nullptr);
    EXPECT_EQ(project.getScenes().size(), 1u);
}

TEST(Project, FindScene_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Scene* added = project.addScene("scene-01", "Old Library");

    EXPECT_EQ(project.findScene("scene-01"), added);
}

TEST(Project, FindScene_UnknownId_ReturnsNullptr)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.findScene("nope"), nullptr);
}

TEST(Project, RemoveScene_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addScene("scene-01", "Old Library");

    project.removeScene("scene-01");

    EXPECT_EQ(project.findScene("scene-01"), nullptr);
    EXPECT_TRUE(project.getScenes().empty());
    EXPECT_TRUE(project.getSceneData().empty());
}

TEST(Project, RemoveScene_UnknownId_IsNoOp)
{
    Core::Project project("My Adventure");
    project.addScene("scene-01", "Old Library");

    EXPECT_NO_THROW(project.removeScene("nope"));
    EXPECT_EQ(project.getScenes().size(), 1u);
}

// --- Character CRUD ---

TEST(Project, AddCharacter_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::Character* character = project.addCharacter("char-01", "Hero");

    ASSERT_NE(character, nullptr);
    EXPECT_EQ(character->getId(), "char-01");
    EXPECT_EQ(project.getCharacters().size(), 1u);
    EXPECT_EQ(project.getCharacterData().size(), 1u);
}

TEST(Project, AddCharacter_DuplicateId_ReturnsNullptr)
{
    Core::Project project("My Adventure");
    project.addCharacter("char-01", "Hero");

    EXPECT_EQ(project.addCharacter("char-01", "Someone Else"), nullptr);
    EXPECT_EQ(project.getCharacters().size(), 1u);
}

TEST(Project, FindCharacter_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Character* added = project.addCharacter("char-01", "Hero");

    EXPECT_EQ(project.findCharacter("char-01"), added);
}

TEST(Project, RemoveCharacter_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addCharacter("char-01", "Hero");

    project.removeCharacter("char-01");

    EXPECT_EQ(project.findCharacter("char-01"), nullptr);
    EXPECT_TRUE(project.getCharacters().empty());
    EXPECT_TRUE(project.getCharacterData().empty());
}

// --- Item CRUD ---

TEST(Project, AddItem_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::Item* item = project.addItem("item-01", "Rusty Key");

    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->getId(), "item-01");
    EXPECT_EQ(project.getItems().size(), 1u);
    EXPECT_EQ(project.getItemData().size(), 1u);
}

TEST(Project, AddItem_DuplicateId_ReturnsNullptr)
{
    Core::Project project("My Adventure");
    project.addItem("item-01", "Rusty Key");

    EXPECT_EQ(project.addItem("item-01", "Another Key"), nullptr);
    EXPECT_EQ(project.getItems().size(), 1u);
}

TEST(Project, FindItem_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Item* added = project.addItem("item-01", "Rusty Key");

    EXPECT_EQ(project.findItem("item-01"), added);
}

TEST(Project, RemoveItem_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addItem("item-01", "Rusty Key");

    project.removeItem("item-01");

    EXPECT_EQ(project.findItem("item-01"), nullptr);
    EXPECT_TRUE(project.getItems().empty());
    EXPECT_TRUE(project.getItemData().empty());
}

// --- Cross-collection independence ---

TEST(Project, DifferentEntityTypes_CanShareTheSameId)
{
    Core::Project project("My Adventure");

    Entities::Scene* scene = project.addScene("shared-id", "A Scene");
    Entities::Character* character = project.addCharacter("shared-id", "A Character");
    Entities::Item* item = project.addItem("shared-id", "An Item");

    EXPECT_NE(scene, nullptr);
    EXPECT_NE(character, nullptr);
    EXPECT_NE(item, nullptr);
}

TEST(Project, RemoveScene_DoesNotAffectCharactersOrItems)
{
    Core::Project project("My Adventure");
    project.addScene("scene-01", "Old Library");
    project.addCharacter("char-01", "Hero");
    project.addItem("item-01", "Rusty Key");

    project.removeScene("scene-01");

    EXPECT_NE(project.findCharacter("char-01"), nullptr);
    EXPECT_NE(project.findItem("item-01"), nullptr);
}
