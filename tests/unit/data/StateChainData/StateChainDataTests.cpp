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

#include "Data/StateChainData.h"

using namespace ADS::Data;
using ADS::Types::StateId;

TEST(StateChainData, DefaultConstruction_HasNoHead)
{
    StateChainData chain;

    EXPECT_FALSE(chain.getHead().has_value());
}

TEST(StateChainData, SetHead_RoundTripsOptionalStateId)
{
    StateChainData chain;

    chain.setHead(StateId(3));

    ASSERT_TRUE(chain.getHead().has_value());
    EXPECT_EQ(chain.getHead(), StateId(3));
}

TEST(StateChainData, SetHead_Nullopt_ClearsHead)
{
    StateChainData chain;
    chain.setHead(StateId(3));

    chain.setHead(std::nullopt);

    EXPECT_FALSE(chain.getHead().has_value());
}
