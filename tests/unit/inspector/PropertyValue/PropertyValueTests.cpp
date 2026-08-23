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

#include "Inspector/PropertyValue.h"

using namespace ADS::Inspector;

TEST(PropertyValue, GetPropertyTypeFromValue_SelectValue_ReturnsSelect)
{
    PropertyValue value = SelectValue({0, 2}, {"A", "B", "C"});

    EXPECT_EQ(getPropertyTypeFromValue(value), PropertyType::Select);
}

TEST(PropertyValue, HoldsType_SelectValue_TrueForSelectValue)
{
    PropertyValue value = SelectValue({}, {});

    EXPECT_TRUE(holdsType<SelectValue>(value));
    EXPECT_FALSE(holdsType<EnumValue>(value));
}

TEST(PropertyValue, GetValueOr_SelectValue_ReturnsStoredValue)
{
    PropertyValue value = SelectValue({1}, {"X", "Y"});

    SelectValue result = getValueOr<SelectValue>(value, SelectValue());

    EXPECT_EQ(result.selectedIndices, (std::vector<int>{1}));
    EXPECT_EQ(result.options, (std::vector<std::string>{"X", "Y"}));
}

TEST(PropertyValue, GetValueOr_SelectValue_WrongType_ReturnsDefault)
{
    PropertyValue value = std::string("not a select value");
    SelectValue fallback({9}, {"fallback"});

    SelectValue result = getValueOr<SelectValue>(value, fallback);

    EXPECT_EQ(result.selectedIndices, (std::vector<int>{9}));
}

TEST(PropertyValue, GetPropertyTypeFromValue_LocalizedText_ReturnsString)
{
    PropertyValue value = LocalizedText{{"es_ES", "Hola"}, {"en_US", "Hello"}};

    EXPECT_EQ(getPropertyTypeFromValue(value), PropertyType::String);
}

TEST(PropertyValue, HoldsType_LocalizedText_TrueForLocalizedText)
{
    PropertyValue value = LocalizedText{{"es_ES", "Hola"}};

    EXPECT_TRUE(holdsType<LocalizedText>(value));
    EXPECT_FALSE(holdsType<std::string>(value));
}

TEST(PropertyValue, GetValueOr_LocalizedText_ReturnsStoredMap)
{
    LocalizedText texts = {{"es_ES", "Hola"}, {"en_US", "Hello"}};
    PropertyValue value = texts;

    LocalizedText result = getValueOr<LocalizedText>(value, LocalizedText{});

    EXPECT_EQ(result, texts);
}
