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

#include "Data/ItemData.h"
#include "Entities/Item.h"

using namespace ADS;

namespace {
    Data::ItemData makeItemData()
    {
        Data::ItemData data;
        data.setId("item-01");
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

TEST(Item, GetPropertyDescriptors_ReturnsNineDescriptors)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    EXPECT_EQ(item.getPropertyDescriptors().size(), 9u);
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

TEST(Item, GetPropertyValue_Quantity_ReturnsInt)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    auto value = item.getPropertyValue("quantity");

    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 1);
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

TEST(Item, SetPropertyValue_Quantity_WrongType_Rejected)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    bool accepted = item.setPropertyValue("quantity", std::string("five"));

    EXPECT_FALSE(accepted);
    EXPECT_EQ(item.getQuantity(), 1);
}

TEST(Item, SetPickableAndUsable_UpdateUnderlyingData)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setPickable(false);
    item.setUsable(true);

    EXPECT_FALSE(item.isPickable());
    EXPECT_TRUE(item.isUsable());
}

TEST(Item, SetQuantity_ChangedValue_FiresEvent)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);
    bool fired = false;
    item.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& e) {
        fired = true;
        EXPECT_EQ(e.propertyId, "quantity");
    });

    item.setQuantity(5);

    EXPECT_TRUE(fired);
    EXPECT_EQ(item.getQuantity(), 5);
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

TEST(Item, SetIconPathAndStartingSceneId_UpdateUnderlyingData)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setIconPath("assets/key.png");
    item.setStartingSceneId("scene-01");

    EXPECT_EQ(item.getIconPath(), "assets/key.png");
    EXPECT_EQ(item.getStartingSceneId(), "scene-01");
}

TEST(Item, GetItemTypeName_ReturnsNameForCurrentIndex)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setItemType(1);

    EXPECT_EQ(item.getItemTypeName(), "Key");
}

TEST(Item, SetDescription_UpdatesUnderlyingData)
{
    Data::ItemData data = makeItemData();
    Entities::Item item(&data);

    item.setDescription("A rusty old key.");

    EXPECT_EQ(item.getDescription(), "A rusty old key.");
}
