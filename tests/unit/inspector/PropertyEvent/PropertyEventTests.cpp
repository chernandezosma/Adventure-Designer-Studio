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

#include "Inspector/PropertyEvent.h"

using namespace ADS::Inspector;

TEST(PropertyEventDispatcher, DefaultConstruction_HasNoSubscribers)
{
    PropertyEventDispatcher dispatcher;

    EXPECT_EQ(dispatcher.getSubscriberCount(), 0u);
}

TEST(PropertyEventDispatcher, Subscribe_IncrementsSubscriberCount)
{
    PropertyEventDispatcher dispatcher;

    dispatcher.subscribe([](const PropertyChangedEvent&) {});

    EXPECT_EQ(dispatcher.getSubscriberCount(), 1u);
}

TEST(PropertyEventDispatcher, Subscribe_ReturnsDistinctHandles)
{
    PropertyEventDispatcher dispatcher;

    SubscriptionHandle first = dispatcher.subscribe([](const PropertyChangedEvent&) {});
    SubscriptionHandle second = dispatcher.subscribe([](const PropertyChangedEvent&) {});

    EXPECT_NE(first, second);
    EXPECT_EQ(dispatcher.getSubscriberCount(), 2u);
}

TEST(PropertyEventDispatcher, Dispatch_InvokesAllSubscribers)
{
    PropertyEventDispatcher dispatcher;
    int callCount = 0;
    dispatcher.subscribe([&callCount](const PropertyChangedEvent&) { ++callCount; });
    dispatcher.subscribe([&callCount](const PropertyChangedEvent&) { ++callCount; });

    dispatcher.dispatch(PropertyChangedEvent("width", 800, 1024, nullptr));

    EXPECT_EQ(callCount, 2);
}

TEST(PropertyEventDispatcher, Dispatch_PassesEventDataToSubscriber)
{
    PropertyEventDispatcher dispatcher;
    PropertyChangedEvent received;

    dispatcher.subscribe([&received](const PropertyChangedEvent& event) { received = event; });
    dispatcher.dispatch(PropertyChangedEvent("width", 800, 1024, nullptr));

    EXPECT_EQ(received.propertyId, "width");
    EXPECT_EQ(std::get<int>(received.oldValue), 800);
    EXPECT_EQ(std::get<int>(received.newValue), 1024);
    EXPECT_EQ(received.source, nullptr);
}

TEST(PropertyEventDispatcher, Unsubscribe_StopsFurtherNotifications)
{
    PropertyEventDispatcher dispatcher;
    int callCount = 0;
    SubscriptionHandle handle = dispatcher.subscribe([&callCount](const PropertyChangedEvent&) { ++callCount; });

    dispatcher.unsubscribe(handle);
    dispatcher.dispatch(PropertyChangedEvent("width", 800, 1024, nullptr));

    EXPECT_EQ(callCount, 0);
    EXPECT_EQ(dispatcher.getSubscriberCount(), 0u);
}

TEST(PropertyEventDispatcher, Unsubscribe_UnknownHandle_IsNoOp)
{
    PropertyEventDispatcher dispatcher;
    dispatcher.subscribe([](const PropertyChangedEvent&) {});

    EXPECT_NO_THROW(dispatcher.unsubscribe(9999));
    EXPECT_EQ(dispatcher.getSubscriberCount(), 1u);
}

TEST(PropertyEventDispatcher, Clear_RemovesAllSubscribers)
{
    PropertyEventDispatcher dispatcher;
    dispatcher.subscribe([](const PropertyChangedEvent&) {});
    dispatcher.subscribe([](const PropertyChangedEvent&) {});

    dispatcher.clear();

    EXPECT_EQ(dispatcher.getSubscriberCount(), 0u);
}

TEST(PropertyChangedEvent, DefaultConstruction_SourceIsNull)
{
    PropertyChangedEvent event;

    EXPECT_EQ(event.source, nullptr);
    EXPECT_TRUE(event.propertyId.empty());
}
