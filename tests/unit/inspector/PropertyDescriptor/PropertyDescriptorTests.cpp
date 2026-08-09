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
