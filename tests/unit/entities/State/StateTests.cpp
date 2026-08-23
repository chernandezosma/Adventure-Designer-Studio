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

#include "Core/Project.h"
#include "Data/StateData.h"
#include "Entities/State.h"

using namespace ADS;
using ADS::Types::StateId;

namespace {
    Data::StateData makeStateData()
    {
        Data::StateData data;
        data.setId(StateId(1));
        data.setName("Locked");
        return data;
    }
}

TEST(State, GetTypeName_ReturnsState)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    EXPECT_EQ(state.getTypeName(), "State");
}

TEST(State, GetStateId_ReturnsTypedId)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    EXPECT_EQ(state.getStateId(), StateId(1));
}

TEST(State, GetId_ReturnsStringifiedId)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    EXPECT_EQ(state.getId(), "1");
}

TEST(State, GetPropertyDescriptors_ReturnsSevenDescriptors)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    auto descriptors = state.getPropertyDescriptors();

    // id(1) + name(1) + next(1) + descriptions(4) = 7
    EXPECT_EQ(descriptors.size(), 7u);
}

TEST(State, GetPropertyDescriptors_IdDescriptorIsReadOnly)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    auto descriptors = state.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& d) { return d.getId() == "id"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_TRUE(it->isReadOnly());
}

TEST(State, GetPropertyValue_Name_ReturnsDataName)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    auto value = state.getPropertyValue("name");

    ASSERT_TRUE(std::holds_alternative<std::string>(value));
    EXPECT_EQ(std::get<std::string>(value), "Locked");
}

TEST(State, SetPropertyValue_Name_ValidType_UpdatesAndReturnsTrue)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    bool accepted = state.setPropertyValue("name", std::string("Broken"));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(data.getName(), "Broken");
}

TEST(State, GetPropertyValue_UnknownId_ReturnsMonostate)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    auto value = state.getPropertyValue("no-such-property");

    EXPECT_TRUE(std::holds_alternative<std::monostate>(value));
}

TEST(State, SetPropertyValue_UnknownId_ReturnsFalse)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    EXPECT_FALSE(state.setPropertyValue("no-such-property", std::string("x")));
}

TEST(State, GetPropertyValue_Next_NoProject_ReturnsEmptySelection)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    auto value = state.getPropertyValue("next");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}

TEST(State, GetPropertyValue_Next_ExcludesSelfAndAlreadyUsedStates)
{
    Core::Project project("My Adventure");
    Entities::State* a = project.addState(StateId(1), "A");
    Entities::State* b = project.addState(StateId(2), "B");
    Entities::State* c = project.addState(StateId(3), "C");
    b->setNext(c->getStateId()); // C is now claimed by B

    // A's own "next" options should list only free states: B is free, C
    // is claimed by B and A isn't editing C's own field, so C is hidden.
    auto value = a->getPropertyValue("next");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    const auto& options = std::get<Inspector::SelectValue>(value).options;
    ASSERT_EQ(options.size(), 1u);
    EXPECT_EQ(options[0], "B");
}

TEST(State, GetPropertyValue_Next_KeepsCurrentlySelectedStateVisible)
{
    Core::Project project("My Adventure");
    Entities::State* a = project.addState(StateId(1), "A");
    Entities::State* b = project.addState(StateId(2), "B");
    a->setNext(b->getStateId());

    // B is "used" (claimed by A's own next), but since it's A's own
    // current value it must stay visible/selectable in A's dropdown.
    auto value = a->getPropertyValue("next");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    const auto& sel = std::get<Inspector::SelectValue>(value);
    ASSERT_EQ(sel.options.size(), 1u);
    EXPECT_EQ(sel.options[0], "B");
    EXPECT_EQ(sel.selectedIndices, (std::vector<int>{0}));
}

TEST(State, GetSetNext_TypedAccessor_RoundTrips)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);

    EXPECT_FALSE(state.getNext().has_value());

    state.setNext(StateId(2));

    ASSERT_TRUE(state.getNext().has_value());
    EXPECT_EQ(state.getNext(), StateId(2));
    EXPECT_EQ(data.getNext(), StateId(2));
}

TEST(State, GetSetDescriptions_RoundTrips)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);
    Data::Descriptions desc{.normal = 1, .longText = 2};

    state.setDescriptions(desc);

    EXPECT_EQ(state.getDescriptions(), desc);
}

TEST(State, GetPropertyValue_DescriptionsNormal_ReturnsLocalizedText)
{
    Data::StateData data = makeStateData();
    Data::DescriptionTexts texts;
    texts.normal["en_US"] = "The door is locked.";
    data.setDescriptionTexts(texts);
    Entities::State state(&data);

    auto value = state.getPropertyValue("descriptionsNormal");

    ASSERT_TRUE(std::holds_alternative<Inspector::LocalizedText>(value));
    EXPECT_EQ(std::get<Inspector::LocalizedText>(value).at("en_US"), "The door is locked.");
}

TEST(State, SetPropertyValue_DescriptionsNormal_UpdatesDescriptionTexts)
{
    Data::StateData data = makeStateData();
    Entities::State state(&data);
    Inspector::LocalizedText text{{"en_US", "The door is locked."}};

    bool accepted = state.setPropertyValue("descriptionsNormal", text);

    EXPECT_TRUE(accepted);
    EXPECT_EQ(data.getDescriptionTexts().normal.at("en_US"), "The door is locked.");
}
