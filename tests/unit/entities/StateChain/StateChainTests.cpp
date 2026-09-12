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

#include <algorithm>

#include "Data/StateChainData.h"
#include "Entities/StateChain.h"

using namespace ADS;
using ADS::Types::ChainId;
using ADS::Types::StateId;

namespace {
    Data::StateChainData makeChainData()
    {
        Data::StateChainData data;
        data.setId(ChainId(1));
        data.setName("Door State");
        return data;
    }
}

TEST(StateChain, GetTypeName_ReturnsChain)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    EXPECT_EQ(chain.getTypeName(), "Chain");
}

TEST(StateChain, GetChainId_ReturnsTypedId)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    EXPECT_EQ(chain.getChainId(), ChainId(1));
}

TEST(StateChain, GetId_ReturnsStringifiedId)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    EXPECT_EQ(chain.getId(), "1");
}

TEST(StateChain, GetPropertyDescriptors_ReturnsThreeDescriptors)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    auto descriptors = chain.getPropertyDescriptors();

    // id(1) + name(1) + head(1) = 3
    EXPECT_EQ(descriptors.size(), 3u);
}

TEST(StateChain, GetPropertyDescriptors_IdDescriptorIsReadOnly)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    auto descriptors = chain.getPropertyDescriptors();
    auto it = std::find_if(descriptors.begin(), descriptors.end(),
        [](const auto& d) { return d.getId() == "id"; });

    ASSERT_NE(it, descriptors.end());
    EXPECT_TRUE(it->isReadOnly());
}

TEST(StateChain, GetPropertyValue_Name_ReturnsDataName)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    auto value = chain.getPropertyValue("name");

    ASSERT_TRUE(std::holds_alternative<std::string>(value));
    EXPECT_EQ(std::get<std::string>(value), "Door State");
}

TEST(StateChain, SetPropertyValue_Name_ValidType_UpdatesAndReturnsTrue)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    bool accepted = chain.setPropertyValue("name", std::string("Window State"));

    EXPECT_TRUE(accepted);
    EXPECT_EQ(data.getName(), "Window State");
}

TEST(StateChain, GetPropertyValue_UnknownId_ReturnsMonostate)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    auto value = chain.getPropertyValue("no-such-property");

    EXPECT_TRUE(std::holds_alternative<std::monostate>(value));
}

TEST(StateChain, SetPropertyValue_UnknownId_ReturnsFalse)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    EXPECT_FALSE(chain.setPropertyValue("no-such-property", std::string("x")));
}

TEST(StateChain, GetPropertyValue_Head_NoProject_ReturnsEmptySelection)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    auto value = chain.getPropertyValue("head");

    ASSERT_TRUE(std::holds_alternative<Inspector::SelectValue>(value));
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).selectedIndices.empty());
    EXPECT_TRUE(std::get<Inspector::SelectValue>(value).options.empty());
}

TEST(StateChain, SetPropertyValue_Head_NoProject_ClearsHead)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    bool accepted = chain.setPropertyValue("head", Inspector::SelectValue({0}, {}));

    EXPECT_TRUE(accepted);
    EXPECT_FALSE(data.getHead().has_value());
}

TEST(StateChain, GetSetHead_TypedAccessor_RoundTrips)
{
    Data::StateChainData data = makeChainData();
    Entities::StateChain chain(&data);

    EXPECT_FALSE(chain.getHead().has_value());

    chain.setHead(StateId(2));

    ASSERT_TRUE(chain.getHead().has_value());
    EXPECT_EQ(chain.getHead(), StateId(2));
    EXPECT_EQ(data.getHead(), StateId(2));
}
