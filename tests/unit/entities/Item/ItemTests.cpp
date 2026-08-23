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
#include "Data/ItemData.h"
#include "Entities/Item.h"

using namespace ADS;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;

namespace {
    Data::ItemData makeItemData()
    {
        Data::ItemData data;
        data.setId(ObjectId(1));
        data.setName("Rusty Key");
        return data;
    }
}

TEST(Item, GetItemTypes_ReturnsEightNames)
{
    const auto& types = Entities::Item::getItemTypes();

    EXPECT_EQ(types.size(), 8u);
    EXPECT_EQ(types[0], "Generic");
}

TEST(Item, GetTypeName_ReturnsItem)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    EXPECT_EQ(item.getTypeName(), "Item");
}

TEST(Item, GetPropertyDescriptors_ReturnsThirtyFiveDescriptors)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    // 31 base descriptors + 4 trigger keys (on_examine/on_item_taken/
    // on_item_dropped/on_item_used)
    EXPECT_EQ(item.getPropertyDescriptors().size(), 35u);
}

TEST(Item, GetPropertyDescriptors_AffordancesIsAffordanceListWithKnownPresets)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto descriptors = item.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
                            [](const auto& d) { return d.getId() == "affordances"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_EQ(it->getType(), Inspector::PropertyType::AffordanceList);
    // "is a container" is the dedicated "isContainer" boolean, not a preset.
    ASSERT_TRUE(it->getOptionsProvider());
    const auto options = it->getOptionsProvider()();
    EXPECT_EQ(options.size(), 24u);
    EXPECT_EQ(options[0], "Takeable");
}

TEST(Item, IsContainer_TogglesAndGatesContainerItemsVisibility)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    // Hidden until the item is a container.
    {
        auto descriptors = item.getPropertyDescriptors();
        auto d = std::find_if(descriptors.begin(), descriptors.end(),
            [](const auto& x) { return x.getId() == "containerItems"; });
        ASSERT_NE(d, descriptors.end());
        EXPECT_FALSE(d->isVisible(&item));
    }

    EXPECT_TRUE(item.setPropertyValue("isContainer", true));
    EXPECT_TRUE(item.isContainer());

    {
        auto descriptors = item.getPropertyDescriptors();
        auto d = std::find_if(descriptors.begin(), descriptors.end(),
            [](const auto& x) { return x.getId() == "containerItems"; });
        ASSERT_NE(d, descriptors.end());
        EXPECT_TRUE(d->isVisible(&item));
    }

    item.setContainer(false);
    EXPECT_FALSE(item.isContainer());
}

TEST(Item, GetPropertyDescriptors_StateDescriptorAllowsCreateNew)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto descriptors = item.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
                            [](const auto& d) { return d.getId() == "state"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_TRUE(it->isAllowCreateNew());
}

TEST(Item, GetPropertyValue_ItemType_ReturnsEnumValueWithOptions)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("itemType");

    ASSERT_TRUE(std::holds_alternative<Inspector::EnumValue>(value));
    Inspector::EnumValue enumValue = std::get<Inspector::EnumValue>(value);
    EXPECT_EQ(enumValue.selectedIndex, 0);
    EXPECT_EQ(enumValue.options, Entities::Item::getItemTypes());
}

TEST(Item, GetPropertyValue_Weight_ReturnsInt)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("weight");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 0);
}

TEST(Item, GetPropertyValue_Slots_ReturnsInt)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("slots");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 1);
}

TEST(Item, GetPropertyValue_ServiceLife_ReturnsInt)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("serviceLife");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 0);
}

TEST(Item, GetPropertyValue_UnknownId_ReturnsMonostate)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    EXPECT_TRUE(std::holds_alternative<std::monostate>(item.getPropertyValue("nope")));
}

TEST(Item, SetPropertyValue_ItemType_ValidEnumValue_UpdatesIndex)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue(
        "itemType", Inspector::EnumValue(2, Entities::Item::getItemTypes()));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(item.getItemType(), 2);
}

TEST(Item, SetPropertyValue_ItemType_WrongType_Rejected)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue("itemType", 2);

    EXPECT_FALSE(accepted);
    EXPECT_EQ(item.getItemType(), 0);
}

TEST(Item, SetPropertyValue_Weight_WrongType_Rejected)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue("weight", std::string("five"));

    EXPECT_FALSE(accepted);
    EXPECT_EQ(item.getWeight(), 0);
}

TEST(Item, Affordances_RoundTripThroughInspectorContract)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    Inspector::AffordanceListValue newValue = {
        {"Takeable", {"on_pickup", "on_drop"}},
        {"Combinable", {}},
    };
    bool accepted = item.setPropertyValue("affordances", newValue);

    EXPECT_TRUE(accepted);
    ASSERT_EQ(item.getAffordances().size(), 2u);
    EXPECT_EQ(item.getAffordances()[0].name, "Takeable");
    EXPECT_EQ(item.getAffordances()[0].triggers, (std::vector<std::string>{"on_pickup", "on_drop"}));
    EXPECT_EQ(item.getAffordances()[1].name, "Combinable");

    auto value = item.getPropertyValue("affordances");
    ASSERT_TRUE(std::holds_alternative<Inspector::AffordanceListValue>(value));
    const auto& list = std::get<Inspector::AffordanceListValue>(value);
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].name, "Takeable");
}

TEST(Item, Affordances_PresetKeyRoundTripsAndReresolvesDisplayName)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    // A preset-derived entry carries its key; a custom-typed one has none.
    Inspector::AffordanceListValue newValue = {
        {"Openable", {"on_open"}, "ITEM.AFF_OPENABLE"},
        {"MyCustomThing", {}},
    };
    ASSERT_TRUE(item.setPropertyValue("affordances", newValue));

    ASSERT_EQ(item.getAffordances().size(), 2u);
    EXPECT_EQ(item.getAffordances()[0].presetKey, "ITEM.AFF_OPENABLE");
    EXPECT_TRUE(item.getAffordances()[1].presetKey.empty());

    // getPropertyValue re-derives the display name from the preset key via
    // translate() rather than trusting whatever text was stored — that's
    // the fix. Compare against the live-translated preset label itself
    // (index 4 == "ITEM.AFF_OPENABLE" in knownAffordanceKeys()) so this
    // assertion holds regardless of which i18n instance/locale is active
    // in the test process.
    auto descriptors = item.getPropertyDescriptors();
    auto affDescriptor = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& d) { return d.getId() == "affordances"; });
    ASSERT_NE(affDescriptor, descriptors.end());
    const auto presetLabels = affDescriptor->getOptionsProvider()();
    auto value = item.getPropertyValue("affordances");
    const auto& list = std::get<Inspector::AffordanceListValue>(value);
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].name, presetLabels[4]);
    EXPECT_EQ(list[0].presetKey, "ITEM.AFF_OPENABLE");
    EXPECT_EQ(list[1].name, "MyCustomThing");
    EXPECT_TRUE(list[1].presetKey.empty());
}

TEST(Item, SetWeight_ChangedValue_FiresEvent)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);
    bool fired = false;
    item.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "weight");
    });

    item.setWeight(200);

    EXPECT_TRUE(fired);
    EXPECT_EQ(item.getWeight(), 200);
}

TEST(Item, SetSlotsAndServiceLife_UpdateUnderlyingData)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setSlots(4);
    item.setServiceLife(75);

    EXPECT_EQ(item.getSlots(), 4);
    EXPECT_EQ(item.getServiceLife(), 75);
}

TEST(Item, SetItemType_ChangedValue_FiresEventWithEnumValuePayload)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);
    Inspector::PropertyValue capturedNew;
    item.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        capturedNew = e.newValue;
    });

    item.setItemType(3);

    ASSERT_TRUE(std::holds_alternative<Inspector::EnumValue>(capturedNew));
    EXPECT_EQ(std::get<Inspector::EnumValue>(capturedNew).selectedIndex, 3);
}

TEST(Item, SetImagePathAndInitialSceneId_UpdateUnderlyingData)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setImagePath("assets/key.png");
    item.setInitialSceneId(SceneId(1));

    EXPECT_EQ(item.getImagePath(), "assets/key.png");
    EXPECT_EQ(item.getInitialSceneId(), SceneId(1));
}

TEST(Item, Synonyms_RoundTripAsNewlineSeparatedText)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue("synonyms", std::string("lantern\nlamp\n"));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(item.getSynonyms(), (std::vector<std::string>{"lantern", "lamp"}));

    auto value = item.getPropertyValue("synonyms");
    ASSERT_TRUE(std::holds_alternative<std::string>(value));
    EXPECT_EQ(std::get<std::string>(value), "lantern\nlamp");
}

TEST(Item, EffectField_RoundTripsThroughInspectorContract)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    EXPECT_TRUE(item.setPropertyValue("dmgBase", 25));
    EXPECT_TRUE(item.setPropertyValue("healRate", 3));

    EXPECT_EQ(item.getDamageEffect().base, 25);
    EXPECT_EQ(item.getHealEffect().rate, 3);
    EXPECT_EQ(std::get<int>(item.getPropertyValue("dmgBase")), 25);
}

TEST(Item, GetPropertyValue_State_NoProject_ReturnsEmptySelection)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("state");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}

TEST(Item, SetPropertyValue_State_NoProject_ClearsState)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue("state", Inspector::SelectValue({0}, {}));

    EXPECT_TRUE(accepted);
    EXPECT_FALSE(data.getState().has_value());
}

TEST(Item, GetSetState_TypedAccessor_RoundTrips)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    EXPECT_FALSE(item.getState().has_value());

    item.setState(ADS::Types::StateId(4));

    ASSERT_TRUE(item.getState().has_value());
    EXPECT_EQ(item.getState(), ADS::Types::StateId(4));
    EXPECT_EQ(data.getState(), ADS::Types::StateId(4));
}

TEST(Item, Project_StateProperty_ResolvesStateThroughProject)
{
    Core::Project project("My Adventure");
    Entities::Item* key = project.addItem(ObjectId(1), "Rusty Key");
    Entities::State* locked = project.addState(ADS::Types::StateId(1), "Locked");

    key->setState(locked->getStateId());
    auto value = key->getPropertyValue("state");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    const auto& sel = std::get<Inspector::SelectValue>(value);
    ASSERT_EQ(sel.options.size(), 1u);
    EXPECT_EQ(sel.options[0], "Locked");
    EXPECT_EQ(sel.selectedIndices, (std::vector<int>{0}));
}

TEST(Item, SlotsConstraint_MaxIsProjectInventoryCapacity)
{
    // Detached item: falls back to the uint8_t ceiling.
    {
        Data::ItemData data = makeItemData();
        Entities::Item item(&data);
        auto descriptors = item.getPropertyDescriptors();
        auto d = std::find_if(descriptors.begin(), descriptors.end(),
            [](const auto& x) { return x.getId() == "slots"; });
        ASSERT_NE(d, descriptors.end());
        EXPECT_EQ(d->getConstraints().maxValue, 255.0f);
    }

    // Attached to a project: max tracks GameData::getInventoryCapacity().
    Core::Project project("My Adventure");
    project.getGameData().setInventoryCapacity(12);
    Entities::Item* key = project.addItem(ObjectId(1), "Rusty Key");

    auto descriptors = key->getPropertyDescriptors();
    auto d = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& x) { return x.getId() == "slots"; });
    ASSERT_NE(d, descriptors.end());
    EXPECT_EQ(d->getConstraints().maxValue, 12.0f);
}

TEST(Item, InitialSceneId_IsASceneDropdownResolvedThroughProject)
{
    Core::Project project("My Adventure");
    project.addScene(SceneId(1), "Old Library");
    Entities::Scene* hallway = project.addScene(SceneId(2), "Dusty Hallway");
    Entities::Item* key = project.addItem(ObjectId(1), "Rusty Key");

    auto descriptors = key->getPropertyDescriptors();
    auto d = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& x) { return x.getId() == "initialSceneId"; });
    ASSERT_NE(d, descriptors.end());
    EXPECT_EQ(d->getType(), Inspector::PropertyType::Select);
    ASSERT_TRUE(d->getOptionsProvider());
    EXPECT_EQ(d->getOptionsProvider()().size(), 2u);

    // Selecting option index 1 == "Dusty Hallway" (SceneId 2).
    EXPECT_TRUE(key->setPropertyValue("initialSceneId", Inspector::SelectValue({1}, {})));
    EXPECT_EQ(key->getInitialSceneId(), hallway->getSceneId());

    auto value = key->getPropertyValue("initialSceneId");
    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_EQ(std::get<Inspector::SelectValue>(value).selectedIndices, (std::vector<int>{1}));

    // Clearing the selection nulls it out.
    EXPECT_TRUE(key->setPropertyValue("initialSceneId", Inspector::SelectValue({}, {})));
    EXPECT_FALSE(key->getInitialSceneId().has_value());
}

TEST(Item, GetItemTypeName_ReturnsNameForCurrentIndex)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setItemType(1);

    EXPECT_EQ(item.getItemTypeName(), "Key");
}

TEST(Item, PropertyValue_DescriptionsNormal_RoundTripsLocalizedTextThroughInspectorContract)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);
    Inspector::LocalizedText texts = {{"es_ES", "Una llave oxidada."}, {"en_US", "A rusty old key."}};

    bool accepted = item.setPropertyValue("descriptionsNormal", texts);

    EXPECT_TRUE(accepted);
    auto value = item.getPropertyValue("descriptionsNormal");
    ASSERT_TRUE(std::holds_alternative<Inspector::LocalizedText>(value));
    EXPECT_EQ(std::get<Inspector::LocalizedText>(value), texts);
}
