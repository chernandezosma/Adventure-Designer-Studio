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

TEST(PropertyConstraints, StringConstraint_MultilineDefaultsToFalse)
{
    PropertyConstraints constraints = PropertyConstraints::string(255);

    EXPECT_FALSE(constraints.multiline);
}

TEST(PropertyConstraints, StringConstraint_MultilineTrue_SetsFlag)
{
    PropertyConstraints constraints = PropertyConstraints::string(255, true);

    EXPECT_TRUE(constraints.multiline);
}

TEST(PropertyConstraints, FilePath_SetsIsFilePathAndExtensions)
{
    std::vector<std::string> extensions = {"png", "jpg", "jpeg", "bmp"};

    PropertyConstraints constraints = PropertyConstraints::filePath(extensions);

    EXPECT_TRUE(constraints.isFilePath);
    EXPECT_EQ(constraints.fileExtensions, extensions);
    EXPECT_FALSE(constraints.multiline);
}

TEST(PropertyConstraints, DefaultConstruction_IsFilePathDefaultsToFalse)
{
    PropertyConstraints constraints;

    EXPECT_FALSE(constraints.isFilePath);
    EXPECT_TRUE(constraints.fileExtensions.empty());
}

TEST(PropertyConstraints, DefaultConstruction_TranslatableDefaultsToFalse)
{
    PropertyConstraints constraints;

    EXPECT_FALSE(constraints.translatable);
}

TEST(PropertyConstraints, TranslatableText_SetsMultilineAndTranslatableAndMaxLength)
{
    PropertyConstraints constraints = PropertyConstraints::translatableText(255);

    EXPECT_TRUE(constraints.translatable);
    EXPECT_TRUE(constraints.multiline);
    ASSERT_TRUE(constraints.maxLength.has_value());
    EXPECT_EQ(*constraints.maxLength, 255u);
}
