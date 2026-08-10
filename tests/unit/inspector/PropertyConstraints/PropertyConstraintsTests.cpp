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

#include "Inspector/PropertyConstraints.h"

using namespace ADS::Inspector;

TEST(PropertyConstraints, DefaultConstruction_HasNoConstraints)
{
    PropertyConstraints constraints;

    EXPECT_FALSE(constraints.hasNumericConstraints());
    EXPECT_FALSE(constraints.hasStringConstraints());
    EXPECT_FALSE(constraints.hasEnumConstraints());
}

TEST(PropertyConstraints, Numeric_SetsMinMaxStepAndFlagsNumeric)
{
    PropertyConstraints constraints = PropertyConstraints::numeric(1.0f, 4096.0f, 2.0f);

    ASSERT_TRUE(constraints.minValue.has_value());
    ASSERT_TRUE(constraints.maxValue.has_value());
    ASSERT_TRUE(constraints.step.has_value());
    EXPECT_FLOAT_EQ(*constraints.minValue, 1.0f);
    EXPECT_FLOAT_EQ(*constraints.maxValue, 4096.0f);
    EXPECT_FLOAT_EQ(*constraints.step, 2.0f);
    EXPECT_TRUE(constraints.hasNumericConstraints());
    EXPECT_FALSE(constraints.hasStringConstraints());
    EXPECT_FALSE(constraints.hasEnumConstraints());
}

TEST(PropertyConstraints, Numeric_DefaultStepIsOne)
{
    PropertyConstraints constraints = PropertyConstraints::numeric(0.0f, 100.0f);

    ASSERT_TRUE(constraints.step.has_value());
    EXPECT_FLOAT_EQ(*constraints.step, 1.0f);
}

TEST(PropertyConstraints, StringConstraint_SetsMaxLengthAndFlagsString)
{
    PropertyConstraints constraints = PropertyConstraints::string(128);

    ASSERT_TRUE(constraints.maxLength.has_value());
    EXPECT_EQ(*constraints.maxLength, 128u);
    EXPECT_TRUE(constraints.hasStringConstraints());
    EXPECT_FALSE(constraints.hasNumericConstraints());
    EXPECT_FALSE(constraints.hasEnumConstraints());
}

TEST(PropertyConstraints, Enumeration_SetsEnumValuesAndFlagsEnum)
{
    std::vector<std::string> options = {"Generic", "Key", "Weapon"};

    PropertyConstraints constraints = PropertyConstraints::enumeration(options);

    EXPECT_EQ(constraints.enumValues, options);
    EXPECT_TRUE(constraints.hasEnumConstraints());
    EXPECT_FALSE(constraints.hasNumericConstraints());
    EXPECT_FALSE(constraints.hasStringConstraints());
}

TEST(PropertyConstraints, Enumeration_EmptyVector_DoesNotFlagEnum)
{
    PropertyConstraints constraints = PropertyConstraints::enumeration({});

    EXPECT_FALSE(constraints.hasEnumConstraints());
}
