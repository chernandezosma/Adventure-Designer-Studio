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
using ADS::Types::CharacterId;
using ADS::Types::ChainId;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;
using ADS::Types::StateId;

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

    Entities::Scene* scene = project.addScene(SceneId(1), "Old Library");

    ASSERT_NE(scene, nullptr);
    EXPECT_EQ(scene->getSceneId(), SceneId(1));
    EXPECT_EQ(scene->getDisplayName(), "Old Library");
    EXPECT_EQ(project.getScenes().size(), 1u);
    EXPECT_EQ(project.getSceneData().size(), 1u);
}

TEST(Project, AddScene_DuplicateId_ReturnsNullptrAndDoesNotAdd)
{
    Core::Project project("My Adventure");
    project.addScene(SceneId(1), "Old Library");

    Entities::Scene* duplicate = project.addScene(SceneId(1), "Another Name");

    EXPECT_EQ(duplicate, nullptr);
    EXPECT_EQ(project.getScenes().size(), 1u);
}

TEST(Project, FindScene_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Scene* added = project.addScene(SceneId(1), "Old Library");

    EXPECT_EQ(project.findScene(SceneId(1)), added);
}

TEST(Project, FindScene_UnknownId_ReturnsNullptr)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.findScene(SceneId(99)), nullptr);
}

TEST(Project, RemoveScene_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addScene(SceneId(1), "Old Library");

    project.removeScene(SceneId(1));

    EXPECT_EQ(project.findScene(SceneId(1)), nullptr);
    EXPECT_TRUE(project.getScenes().empty());
    EXPECT_TRUE(project.getSceneData().empty());
}

TEST(Project, RemoveScene_UnknownId_IsNoOp)
{
    Core::Project project("My Adventure");
    project.addScene(SceneId(1), "Old Library");

    EXPECT_NO_THROW(project.removeScene(SceneId(99)));
    EXPECT_EQ(project.getScenes().size(), 1u);
}

// --- Character CRUD ---

TEST(Project, AddCharacter_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::Character* character = project.addCharacter(CharacterId(1), "Hero");

    ASSERT_NE(character, nullptr);
    EXPECT_EQ(character->getCharacterId(), CharacterId(1));
    EXPECT_EQ(project.getCharacters().size(), 1u);
    EXPECT_EQ(project.getCharacterData().size(), 1u);
}

TEST(Project, AddCharacter_DuplicateId_ReturnsNullptr)
{
    Core::Project project("My Adventure");
    project.addCharacter(CharacterId(1), "Hero");

    EXPECT_EQ(project.addCharacter(CharacterId(1), "Someone Else"), nullptr);
    EXPECT_EQ(project.getCharacters().size(), 1u);
}

TEST(Project, FindCharacter_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Character* added = project.addCharacter(CharacterId(1), "Hero");

    EXPECT_EQ(project.findCharacter(CharacterId(1)), added);
}

TEST(Project, RemoveCharacter_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addCharacter(CharacterId(1), "Hero");

    project.removeCharacter(CharacterId(1));

    EXPECT_EQ(project.findCharacter(CharacterId(1)), nullptr);
    EXPECT_TRUE(project.getCharacters().empty());
    EXPECT_TRUE(project.getCharacterData().empty());
}

// --- Item CRUD ---

TEST(Project, AddItem_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::Item* item = project.addItem(ObjectId(1), "Rusty Key");

    ASSERT_NE(item, nullptr);
    EXPECT_EQ(item->getItemId(), ObjectId(1));
    EXPECT_EQ(project.getItems().size(), 1u);
    EXPECT_EQ(project.getItemData().size(), 1u);
}

TEST(Project, AddItem_DuplicateId_ReturnsNullptr)
{
    Core::Project project("My Adventure");
    project.addItem(ObjectId(1), "Rusty Key");

    EXPECT_EQ(project.addItem(ObjectId(1), "Another Key"), nullptr);
    EXPECT_EQ(project.getItems().size(), 1u);
}

TEST(Project, FindItem_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::Item* added = project.addItem(ObjectId(1), "Rusty Key");

    EXPECT_EQ(project.findItem(ObjectId(1)), added);
}

TEST(Project, RemoveItem_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addItem(ObjectId(1), "Rusty Key");

    project.removeItem(ObjectId(1));

    EXPECT_EQ(project.findItem(ObjectId(1)), nullptr);
    EXPECT_TRUE(project.getItems().empty());
    EXPECT_TRUE(project.getItemData().empty());
}

// --- State CRUD ---

TEST(Project, AddState_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::State* state = project.addState(StateId(1), "Locked");

    ASSERT_NE(state, nullptr);
    EXPECT_EQ(state->getStateId(), StateId(1));
    EXPECT_EQ(state->getDisplayName(), "Locked");
    EXPECT_EQ(project.getStates().size(), 1u);
    EXPECT_EQ(project.getStateData().size(), 1u);
}

TEST(Project, AddState_DuplicateId_ReturnsNullptrAndDoesNotAdd)
{
    Core::Project project("My Adventure");
    project.addState(StateId(1), "Locked");

    Entities::State* duplicate = project.addState(StateId(1), "Another Name");

    EXPECT_EQ(duplicate, nullptr);
    EXPECT_EQ(project.getStates().size(), 1u);
}

TEST(Project, FindState_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::State* added = project.addState(StateId(1), "Locked");

    EXPECT_EQ(project.findState(StateId(1)), added);
}

TEST(Project, FindState_UnknownId_ReturnsNullptr)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.findState(StateId(99)), nullptr);
}

TEST(Project, RemoveState_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addState(StateId(1), "Locked");

    project.removeState(StateId(1));

    EXPECT_EQ(project.findState(StateId(1)), nullptr);
    EXPECT_TRUE(project.getStates().empty());
    EXPECT_TRUE(project.getStateData().empty());
}

TEST(Project, RemoveState_UnknownId_IsNoOp)
{
    Core::Project project("My Adventure");
    project.addState(StateId(1), "Locked");

    EXPECT_NO_THROW(project.removeState(StateId(99)));
    EXPECT_EQ(project.getStates().size(), 1u);
}

// --- StateChain CRUD ---

TEST(Project, AddChain_NewId_ReturnsEntityAndAddsToCollections)
{
    Core::Project project("My Adventure");

    Entities::StateChain* chain = project.addChain(ChainId(1), "Door State");

    ASSERT_NE(chain, nullptr);
    EXPECT_EQ(chain->getChainId(), ChainId(1));
    EXPECT_EQ(chain->getDisplayName(), "Door State");
    EXPECT_EQ(project.getChains().size(), 1u);
    EXPECT_EQ(project.getChainData().size(), 1u);
}

TEST(Project, AddChain_DuplicateId_ReturnsNullptrAndDoesNotAdd)
{
    Core::Project project("My Adventure");
    project.addChain(ChainId(1), "Door State");

    Entities::StateChain* duplicate = project.addChain(ChainId(1), "Another Name");

    EXPECT_EQ(duplicate, nullptr);
    EXPECT_EQ(project.getChains().size(), 1u);
}

TEST(Project, FindChain_ExistingId_ReturnsSamePointerAsAdd)
{
    Core::Project project("My Adventure");
    Entities::StateChain* added = project.addChain(ChainId(1), "Door State");

    EXPECT_EQ(project.findChain(ChainId(1)), added);
}

TEST(Project, FindChain_UnknownId_ReturnsNullptr)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.findChain(ChainId(99)), nullptr);
}

TEST(Project, RemoveChain_ExistingId_RemovesFromBothCollections)
{
    Core::Project project("My Adventure");
    project.addChain(ChainId(1), "Door State");

    project.removeChain(ChainId(1));

    EXPECT_EQ(project.findChain(ChainId(1)), nullptr);
    EXPECT_TRUE(project.getChains().empty());
    EXPECT_TRUE(project.getChainData().empty());
}

TEST(Project, RemoveChain_UnknownId_IsNoOp)
{
    Core::Project project("My Adventure");
    project.addChain(ChainId(1), "Door State");

    EXPECT_NO_THROW(project.removeChain(ChainId(99)));
    EXPECT_EQ(project.getChains().size(), 1u);
}

// --- getFreeStates ---

TEST(Project, GetFreeStates_NoStatesUsedYet_ReturnsAll)
{
    Core::Project project("My Adventure");
    project.addState(StateId(1), "Locked");
    project.addState(StateId(2), "Broken");

    auto free = project.getFreeStates();

    EXPECT_EQ(free.size(), 2u);
}

TEST(Project, GetFreeStates_ExcludesStateUsedAsAnotherStatesNext)
{
    Core::Project project("My Adventure");
    Entities::State* locked = project.addState(StateId(1), "Locked");
    Entities::State* broken = project.addState(StateId(2), "Broken");
    locked->setNext(broken->getStateId());

    auto free = project.getFreeStates();

    ASSERT_EQ(free.size(), 1u);
    EXPECT_EQ(free[0]->getStateId(), locked->getStateId());
}

TEST(Project, GetFreeStates_ExcludesStateUsedAsChainHead)
{
    Core::Project project("My Adventure");
    Entities::State* locked = project.addState(StateId(1), "Locked");
    project.addChain(ChainId(1), "Door State")->setHead(locked->getStateId());

    auto free = project.getFreeStates();

    EXPECT_TRUE(free.empty());
}

TEST(Project, GetFreeStates_KeepEvenIfUsed_IncludesThatOneAnyway)
{
    Core::Project project("My Adventure");
    Entities::State* locked = project.addState(StateId(1), "Locked");
    project.addChain(ChainId(1), "Door State")->setHead(locked->getStateId());

    auto free = project.getFreeStates(std::nullopt, locked->getStateId());

    ASSERT_EQ(free.size(), 1u);
    EXPECT_EQ(free[0]->getStateId(), locked->getStateId());
}

TEST(Project, GetFreeStates_ExcludeSelf_OmitsGivenId)
{
    Core::Project project("My Adventure");
    Entities::State* locked = project.addState(StateId(1), "Locked");
    project.addState(StateId(2), "Broken");

    auto free = project.getFreeStates(locked->getStateId());

    ASSERT_EQ(free.size(), 1u);
    EXPECT_NE(free[0]->getStateId(), locked->getStateId());
}

// --- Scene state resolution (cross-entity, via a live Project) ---
//
// A Scene's "state" references a State directly (not a Chain — Chain
// exists purely to link a State's own "next"/"head" chain internally, and
// is never itself selectable from a Scene; see State::buildStateOptionLabels()
// vs. Scene::buildStateOptionLabels()). Unlike a State's own "next" field,
// it isn't restricted to "free" states — many scenes may reference the
// same state.

TEST(Project, Scene_StateProperty_ResolvesStateThroughProject)
{
    Core::Project project("My Adventure");
    Entities::Scene* scene = project.addScene(SceneId(1), "Old Library");
    Entities::State* locked = project.addState(StateId(1), "Locked");

    scene->setState(locked->getStateId());
    auto value = scene->getPropertyValue("state");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    const auto& sel = std::get<Inspector::SelectValue>(value);
    ASSERT_EQ(sel.options.size(), 1u);
    EXPECT_EQ(sel.options[0], "Locked");
    EXPECT_EQ(sel.selectedIndices, (std::vector<int>{0}));
}

TEST(Project, Scene_StateProperty_TwoScenesCanShareTheSameState)
{
    Core::Project project("My Adventure");
    Entities::Scene* library = project.addScene(SceneId(1), "Library");
    Entities::Scene* hallway = project.addScene(SceneId(2), "Hallway");
    Entities::State* locked = project.addState(StateId(1), "Locked");

    library->setState(locked->getStateId());
    hallway->setState(locked->getStateId());

    EXPECT_EQ(library->getState(), locked->getStateId());
    EXPECT_EQ(hallway->getState(), locked->getStateId());
}

TEST(Project, CreateStateFlow_OnSceneStateField_CreatesUnlinkedStateLeavesSelectionAlone)
{
    // Replicates the Inspector's "+" button (InspectorPanel::onCreateState,
    // called from renderProperty()'s createNewRequested branch): it fires
    // the exact same creation as the tree's "Add > State" and leaves the
    // originating field's own selection untouched.
    Core::Project project("My Adventure");
    Entities::Scene* scene = project.addScene(SceneId(1), "Room");
    project.addState(StateId(10), "Locked");
    project.addState(StateId(11), "Broken");

    EXPECT_EQ(project.getStates().size(), 2u);

    Entities::State* newState = project.addState(StateId(12), "Pristine");
    ASSERT_NE(newState, nullptr);

    // The field's own selection is untouched by creation alone.
    auto value = scene->getPropertyValue("state");
    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());

    // The user then picks the new state from the dropdown themselves.
    const auto& states = project.getStates();
    int idx = -1;
    for (size_t i = 0; i < states.size(); ++i)
        if (states[i]->getStateId() == newState->getStateId()) idx = static_cast<int>(i);
    ASSERT_GE(idx, 0);

    bool accepted = scene->setPropertyValue("state", Inspector::SelectValue({idx}, {}));
    EXPECT_TRUE(accepted);
    EXPECT_EQ(scene->getState(), newState->getStateId());
}

TEST(Project, CreateStateDialogFlow_OnStateOwnNextField_LinksDirectlyNoChain)
{
    // Regression: a State entity's own "next" field (and StateChain's own
    // "head" field) reference a State *directly*, not via a Chain wrapper.
    // InspectorPanel::renderCreateStateDialog()'s OK handler must detect
    // this and call the typed State::setNext()/StateChain::setHead()
    // accessor instead of wrapping the new state in a same-named Chain
    // (which is only correct for reference fields like Scene's "state").
    Core::Project project("My Adventure");
    Entities::State* locked = project.addState(StateId(1), "Locked");
    ASSERT_NE(locked, nullptr);

    // Before: with only one state, "next" has no free options — this is
    // the exact "No options available" case the user reported, except
    // here it's the *expected* state (nothing else exists yet) and the
    // "+" button (setAllowCreateNew()) is what lets them grow the chain
    // from right here instead of leaving to the tree panel.
    {
        auto value = locked->getPropertyValue("next");
        ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
        EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
    }

    // Replicate renderCreateStateDialog()'s OK handler for the
    // direct-state-field branch: create "Broken" and link it as
    // Locked's own "next" — no Chain should be created.
    Entities::State* newState = project.addState(StateId(2), "Broken");
    ASSERT_NE(newState, nullptr);
    locked->setNext(newState->getStateId());

    EXPECT_EQ(project.getChains().size(), 0u);
    ASSERT_TRUE(locked->getNext().has_value());
    EXPECT_EQ(*locked->getNext(), newState->getStateId());

    auto value = locked->getPropertyValue("next");
    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    const auto& sel = std::get<Inspector::SelectValue>(value);
    ASSERT_EQ(sel.options.size(), 1u);
    EXPECT_EQ(sel.options[0], "Broken");
    EXPECT_EQ(sel.selectedIndices, (std::vector<int>{0}));
}

// --- Cross-collection independence ---

TEST(Project, DifferentEntityTypes_CanShareTheSameRawIdValue)
{
    Core::Project project("My Adventure");

    Entities::Scene* scene = project.addScene(SceneId(1), "A Scene");
    Entities::Character* character = project.addCharacter(CharacterId(1), "A Character");
    Entities::Item* item = project.addItem(ObjectId(1), "An Item");

    EXPECT_NE(scene, nullptr);
    EXPECT_NE(character, nullptr);
    EXPECT_NE(item, nullptr);
}

TEST(Project, RemoveScene_DoesNotAffectCharactersOrItems)
{
    Core::Project project("My Adventure");
    project.addScene(SceneId(1), "Old Library");
    project.addCharacter(CharacterId(1), "Hero");
    project.addItem(ObjectId(1), "Rusty Key");

    project.removeScene(SceneId(1));

    EXPECT_NE(project.findCharacter(CharacterId(1)), nullptr);
    EXPECT_NE(project.findItem(ObjectId(1)), nullptr);
}

TEST(Project, GameData_DefaultInventoryCapacity_IsTwenty)
{
    Core::Project project("My Adventure");

    EXPECT_EQ(project.getGameData().getInventoryCapacity(), 20);
}

TEST(Project, GameData_SetInventoryCapacity_RoundTripsAndFloorsAtOne)
{
    Core::Project project("My Adventure");

    project.getGameData().setInventoryCapacity(30);
    EXPECT_EQ(project.getGameData().getInventoryCapacity(), 30);

    project.getGameData().setInventoryCapacity(0);
    EXPECT_EQ(project.getGameData().getInventoryCapacity(), 1);
}