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

#include "Data/BaseData.h"

using namespace ADS::Data;

TEST(BaseData, DefaultConstruction_HasEmptyIdAndName)
{
    BaseData data;

    EXPECT_TRUE(data.getId().empty());
    EXPECT_TRUE(data.getName().empty());
}

TEST(BaseData, SetId_UpdatesId)
{
    BaseData data;

    data.setId("scene-01");

    EXPECT_EQ(data.getId(), "scene-01");
}

TEST(BaseData, SetName_UpdatesName)
{
    BaseData data;

    data.setName("Old Library");

    EXPECT_EQ(data.getName(), "Old Library");
}

TEST(BaseData, SetId_DoesNotAffectName)
{
    BaseData data;
    data.setName("Old Library");

    data.setId("scene-01");

    EXPECT_EQ(data.getName(), "Old Library");
}