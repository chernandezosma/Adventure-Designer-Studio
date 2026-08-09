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
#include "Entities/BaseEntity.h"

using namespace ADS;

namespace {
    // BaseEntity itself is abstract (IInspectable's getTypeName/
    // getPropertyDescriptors/getPropertyValue/setPropertyValue are pure
    // virtual). This minimal subclass exists purely to exercise the
    // concrete BaseEntity machinery (id/name/event dispatch) in isolation.
    class TestableEntity : public Entities::BaseEntity {
    public:
        explicit TestableEntity(Data::BaseData* data) : BaseEntity(data) {}

        // LCOV_EXCL_START — unused IInspectable stubs, only needed to make the class concrete
        std::string getTypeName() const override { return "Testable"; }
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override { return {}; }
        Inspector::PropertyValue getPropertyValue(const std::string&) const override { return std::monostate{}; }
        bool setPropertyValue(const std::string&, const Inspector::PropertyValue&) override { return false; }
        // LCOV_EXCL_STOP
    };
}

TEST(BaseEntity, Construction_ReadsIdAndNameFromDataObject)
{
    Data::BaseData data;
    data.setId("entity-01");
    data.setName("Some Entity");

    TestableEntity entity(&data);

    EXPECT_EQ(entity.getId(), "entity-01");
    EXPECT_EQ(entity.getDisplayName(), "Some Entity");
}

TEST(BaseEntity, GetEventDispatcher_ReturnsUsableDispatcher)
{
    Data::BaseData data;
    TestableEntity entity(&data);

    EXPECT_EQ(entity.getEventDispatcher().getSubscriberCount(), 0u);
}

TEST(BaseEntity, SetName_ChangedValue_UpdatesDataAndFiresEvent)
{
    Data::BaseData data;
    data.setName("Old Name");
    TestableEntity entity(&data);

    bool fired = false;
    std::string capturedPropertyId;
    entity.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent& event) {
        fired = true;
        capturedPropertyId = event.propertyId;
    });

    entity.setName("New Name");

    EXPECT_EQ(data.getName(), "New Name");
    EXPECT_EQ(entity.getDisplayName(), "New Name");
    EXPECT_TRUE(fired);
    EXPECT_EQ(capturedPropertyId, "name");
}

TEST(BaseEntity, SetName_SameValue_DoesNotFireEvent)
{
    Data::BaseData data;
    data.setName("Same Name");
    TestableEntity entity(&data);

    bool fired = false;
    entity.getEventDispatcher().subscribe([&](const Inspector::PropertyChangedEvent&) { fired = true; });

    entity.setName("Same Name");

    EXPECT_FALSE(fired);
}
