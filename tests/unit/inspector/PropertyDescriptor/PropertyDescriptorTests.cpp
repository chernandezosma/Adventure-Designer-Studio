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

#include "Inspector/PropertyDescriptor.h"

using namespace ADS::Inspector;

TEST(PropertyDescriptor, Construction_SetsIdDisplayNameAndType)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    EXPECT_EQ(descriptor.getId(), "width");
    EXPECT_EQ(descriptor.getDisplayName(), "Width");
    EXPECT_EQ(descriptor.getType(), PropertyType::Int);
}

TEST(PropertyDescriptor, Defaults_EmptyDescriptionCategoryNotReadOnly)
{
    PropertyDescriptor descriptor("id", "ID", PropertyType::String);

    EXPECT_TRUE(descriptor.getDescription().empty());
    EXPECT_TRUE(descriptor.getCategory().empty());
    EXPECT_FALSE(descriptor.isReadOnly());
}

TEST(PropertyDescriptor, SetDescription_UpdatesDescriptionAndReturnsSelf)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    PropertyDescriptor& ref = descriptor.setDescription("Scene width in pixels");

    EXPECT_EQ(descriptor.getDescription(), "Scene width in pixels");
    EXPECT_EQ(&ref, &descriptor);
}

TEST(PropertyDescriptor, SetCategory_UpdatesCategory)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    descriptor.setCategory("Dimensions");

    EXPECT_EQ(descriptor.getCategory(), "Dimensions");
}

TEST(PropertyDescriptor, SetDescription_LvalueOverload_UpdatesDescription)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    std::string desc = "Scene width in pixels";

    descriptor.setDescription(desc);

    EXPECT_EQ(descriptor.getDescription(), "Scene width in pixels");
}

TEST(PropertyDescriptor, SetCategory_LvalueOverload_UpdatesCategory)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    std::string category = "Dimensions";

    descriptor.setCategory(category);

    EXPECT_EQ(descriptor.getCategory(), "Dimensions");
}

TEST(PropertyDescriptor, SetConstraints_LvalueOverload_UpdatesConstraints)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    PropertyConstraints constraints = PropertyConstraints::numeric(1, 4096, 1);

    descriptor.setConstraints(constraints);

    EXPECT_TRUE(descriptor.getConstraints().hasNumericConstraints());
}

TEST(PropertyDescriptor, SetVisibilityCondition_LvalueOverload_UpdatesCondition)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    std::function<bool(const IInspectable*)> condition = [](const IInspectable*) { return false; };

    descriptor.setVisibilityCondition(condition);

    EXPECT_FALSE(descriptor.isVisible(nullptr));
}

TEST(PropertyDescriptor, SetConstraints_UpdatesConstraints)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    descriptor.setConstraints(PropertyConstraints::numeric(1, 4096, 1));

    EXPECT_TRUE(descriptor.getConstraints().hasNumericConstraints());
    EXPECT_FLOAT_EQ(*descriptor.getConstraints().minValue, 1.0f);
}

TEST(PropertyDescriptor, SetReadOnly_DefaultsToTrue)
{
    PropertyDescriptor descriptor("id", "ID", PropertyType::String);

    descriptor.setReadOnly();

    EXPECT_TRUE(descriptor.isReadOnly());
}

TEST(PropertyDescriptor, SetReadOnly_ExplicitFalse)
{
    PropertyDescriptor descriptor("id", "ID", PropertyType::String);

    descriptor.setReadOnly(true);
    descriptor.setReadOnly(false);

    EXPECT_FALSE(descriptor.isReadOnly());
}

TEST(PropertyDescriptor, FluentChain_AllSettersReturnSameInstance)
{
    PropertyDescriptor descriptor =
        PropertyDescriptor("width", "Width", PropertyType::Int)
            .setCategory("Dimensions")
            .setDescription("Scene width in pixels")
            .setConstraints(PropertyConstraints::numeric(1, 4096, 1))
            .setReadOnly(false);

    EXPECT_EQ(descriptor.getCategory(), "Dimensions");
    EXPECT_EQ(descriptor.getDescription(), "Scene width in pixels");
    EXPECT_TRUE(descriptor.getConstraints().hasNumericConstraints());
    EXPECT_FALSE(descriptor.isReadOnly());
}

TEST(PropertyDescriptor, IsVisible_NoConditionSet_ReturnsTrue)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    EXPECT_TRUE(descriptor.isVisible(nullptr));
}

TEST(PropertyDescriptor, IsVisible_ConditionSet_EvaluatesCallback)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    descriptor.setVisibilityCondition([](const IInspectable*) { return false; });

    EXPECT_FALSE(descriptor.isVisible(nullptr));
}

TEST(PropertyDescriptor, IsMultiSelect_DefaultsToFalse)
{
    PropertyDescriptor descriptor("items", "Items", PropertyType::Select);

    EXPECT_FALSE(descriptor.isMultiSelect());
}

TEST(PropertyDescriptor, SetMultiSelect_DefaultArgument_SetsTrue)
{
    PropertyDescriptor descriptor("items", "Items", PropertyType::Select);

    descriptor.setMultiSelect();

    EXPECT_TRUE(descriptor.isMultiSelect());
}

TEST(PropertyDescriptor, SetMultiSelect_ExplicitFalse)
{
    PropertyDescriptor descriptor("items", "Items", PropertyType::Select);

    descriptor.setMultiSelect(true);
    descriptor.setMultiSelect(false);

    EXPECT_FALSE(descriptor.isMultiSelect());
}

TEST(PropertyDescriptor, IsAllowCreateNew_DefaultsToFalse)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);

    EXPECT_FALSE(descriptor.isAllowCreateNew());
}

TEST(PropertyDescriptor, SetAllowCreateNew_DefaultArgument_SetsTrue)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);

    descriptor.setAllowCreateNew();

    EXPECT_TRUE(descriptor.isAllowCreateNew());
}

TEST(PropertyDescriptor, SetAllowCreateNew_ExplicitFalse)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);

    descriptor.setAllowCreateNew(true);
    descriptor.setAllowCreateNew(false);

    EXPECT_FALSE(descriptor.isAllowCreateNew());
}

TEST(PropertyDescriptor, GetOptionsProvider_DefaultsToEmpty)
{
    PropertyDescriptor descriptor("items", "Items", PropertyType::Select);

    EXPECT_FALSE(static_cast<bool>(descriptor.getOptionsProvider()));
}

TEST(PropertyDescriptor, SetOptionsProvider_InvokedByGetOptionsProvider)
{
    PropertyDescriptor descriptor("items", "Items", PropertyType::Select);

    descriptor.setOptionsProvider([] { return std::vector<std::string>{"A", "B"}; });

    ASSERT_TRUE(static_cast<bool>(descriptor.getOptionsProvider()));
    EXPECT_EQ(descriptor.getOptionsProvider()(), (std::vector<std::string>{"A", "B"}));
}

TEST(PropertyDescriptor, IsUserDefinedOption_DefaultsToFalseForEveryIndex)
{
    PropertyDescriptor descriptor("affordances", "Affordances", PropertyType::Select);

    EXPECT_FALSE(descriptor.isUserDefinedOption(0));
    EXPECT_FALSE(descriptor.isUserDefinedOption(3));
}

TEST(PropertyDescriptor, GetUserDefinedOptionBit_UnsetIndex_ReturnsZero)
{
    PropertyDescriptor descriptor("affordances", "Affordances", PropertyType::Select);

    EXPECT_EQ(descriptor.getUserDefinedOptionBit(3), 0);
}

TEST(PropertyDescriptor, SetUserDefinedOptionBits_MarksOnlyGivenIndices)
{
    PropertyDescriptor descriptor("affordances", "Affordances", PropertyType::Select);

    descriptor.setUserDefinedOptionBits({{3, 6}, {4, 7}});

    EXPECT_FALSE(descriptor.isUserDefinedOption(0));
    EXPECT_FALSE(descriptor.isUserDefinedOption(2));
    ASSERT_TRUE(descriptor.isUserDefinedOption(3));
    EXPECT_EQ(descriptor.getUserDefinedOptionBit(3), 6);
    ASSERT_TRUE(descriptor.isUserDefinedOption(4));
    EXPECT_EQ(descriptor.getUserDefinedOptionBit(4), 7);
}

TEST(PropertyDescriptor, IsVisible_ConditionReceivesTarget)
{
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);
    const IInspectable* seenTarget = reinterpret_cast<const IInspectable*>(0x1);
    const IInspectable* capturedTarget = nullptr;

    descriptor.setVisibilityCondition([&capturedTarget](const IInspectable* target) {
        capturedTarget = target;
        return true;
    });

    descriptor.isVisible(seenTarget);

    EXPECT_EQ(capturedTarget, seenTarget);
}

TEST(PropertyDescriptor, GetSubcategory_DefaultsToEmpty)
{
    PropertyDescriptor descriptor("dmgBase", "Base", PropertyType::Int);

    EXPECT_TRUE(descriptor.getSubcategory().empty());
}

TEST(PropertyDescriptor, SetSubcategory_UpdatesValueAndReturnsSelf)
{
    PropertyDescriptor descriptor("dmgBase", "Base", PropertyType::Int);

    PropertyDescriptor& ref = descriptor.setSubcategory("Damage");

    EXPECT_EQ(descriptor.getSubcategory(), "Damage");
    EXPECT_EQ(&ref, &descriptor);
}

TEST(PropertyDescriptor, SetSubcategory_LvalueOverload_UpdatesValue)
{
    PropertyDescriptor descriptor("healBase", "Base", PropertyType::Int);
    const std::string group = "Heal";

    descriptor.setSubcategory(group);

    EXPECT_EQ(descriptor.getSubcategory(), "Heal");
}

TEST(PropertyDescriptor, GetEmptyOptionsTextKey_DefaultsToGenericKey)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);

    EXPECT_EQ(descriptor.getEmptyOptionsTextKey(), "INSPECTOR.NO_OPTIONS");
}

TEST(PropertyDescriptor, SetEmptyOptionsText_OverridesKeyAndReturnsSelf)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);

    PropertyDescriptor& ref = descriptor.setEmptyOptionsText("INSPECTOR.NO_STATES");

    EXPECT_EQ(descriptor.getEmptyOptionsTextKey(), "INSPECTOR.NO_STATES");
    EXPECT_EQ(&ref, &descriptor);
}

TEST(PropertyDescriptor, SetEmptyOptionsText_LvalueOverload_OverridesKey)
{
    PropertyDescriptor descriptor("state", "State", PropertyType::Select);
    const std::string key = "INSPECTOR.NO_STATES";

    descriptor.setEmptyOptionsText(key);

    EXPECT_EQ(descriptor.getEmptyOptionsTextKey(), "INSPECTOR.NO_STATES");
}
