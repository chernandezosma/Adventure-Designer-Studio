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
using ADS::Types::SceneId;
using ADS::Types::SceneTag;

TEST(BaseData, DefaultConstruction_HasUnassignedIdAndEmptyName)
{
    BaseData<SceneTag> data;

    EXPECT_EQ(data.getId().value, 0);
    EXPECT_TRUE(data.getName().empty());
}

TEST(BaseData, SetId_UpdatesId)
{
    BaseData<SceneTag> data;

    data.setId(SceneId(1));

    EXPECT_EQ(data.getId(), SceneId(1));
}

TEST(BaseData, GetIdString_ReturnsStringifiedId)
{
    BaseData<SceneTag> data;

    data.setId(SceneId(7));

    EXPECT_EQ(data.getIdString(), "7");
}

TEST(BaseData, SetName_UpdatesName)
{
    BaseData<SceneTag> data;

    data.setName("Old Library");

    EXPECT_EQ(data.getName(), "Old Library");
}

TEST(BaseData, SetId_DoesNotAffectName)
{
    BaseData<SceneTag> data;
    data.setName("Old Library");

    data.setId(SceneId(1));

    EXPECT_EQ(data.getName(), "Old Library");
}