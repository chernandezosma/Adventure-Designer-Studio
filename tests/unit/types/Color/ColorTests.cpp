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

#include "Types/Color.h"

using namespace ADS::Types;

TEST(Color, DefaultConstruction_IsOpaqueWhite)
{
    Color color;

    EXPECT_FLOAT_EQ(color.r, 1.0f);
    EXPECT_FLOAT_EQ(color.g, 1.0f);
    EXPECT_FLOAT_EQ(color.b, 1.0f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);
}

TEST(Color, ConstructionFromRGBA_StoresAllChannels)
{
    Color color(0.1f, 0.2f, 0.3f, 0.4f);

    EXPECT_FLOAT_EQ(color.r, 0.1f);
    EXPECT_FLOAT_EQ(color.g, 0.2f);
    EXPECT_FLOAT_EQ(color.b, 0.3f);
    EXPECT_FLOAT_EQ(color.a, 0.4f);
}

TEST(Color, ConstructionFromRGB_DefaultsAlphaToOpaque)
{
    Color color(0.5f, 0.6f, 0.7f);

    EXPECT_FLOAT_EQ(color.a, 1.0f);
}

TEST(Color, Equality_SameValues_ReturnsTrue)
{
    Color a(0.1f, 0.2f, 0.3f, 0.4f);
    Color b(0.1f, 0.2f, 0.3f, 0.4f);

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(Color, Inequality_DifferentRed_ReturnsTrue)
{
    Color a(0.1f, 0.2f, 0.3f, 0.4f);
    Color b(0.9f, 0.2f, 0.3f, 0.4f);

    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
}

TEST(Color, Inequality_DifferentGreen_ReturnsTrue)
{
    Color a(0.1f, 0.2f, 0.3f, 0.4f);
    Color b(0.1f, 0.9f, 0.3f, 0.4f);

    EXPECT_TRUE(a != b);
}

TEST(Color, Inequality_DifferentBlue_ReturnsTrue)
{
    Color a(0.1f, 0.2f, 0.3f, 0.4f);
    Color b(0.1f, 0.2f, 0.9f, 0.4f);

    EXPECT_TRUE(a != b);
}

TEST(Color, Inequality_DifferentAlpha_ReturnsTrue)
{
    Color a(0.1f, 0.2f, 0.3f, 0.4f);
    Color b(0.1f, 0.2f, 0.3f, 0.9f);

    EXPECT_TRUE(a != b);
}