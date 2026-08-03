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

#ifndef ADS_IINSPECTABLE_H
#define ADS_IINSPECTABLE_H

#include <string>
#include <vector>
#include "PropertyDescriptor.h"
#include "PropertyValue.h"
#include "PropertyEvent.h"

namespace ADS::Inspector {
    /**
     * @brief Interface for objects that can be inspected in the property inspector
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Any object that should be editable through the inspector panel must
     * implement this interface. It provides methods for querying property
     * metadata, getting and setting property values, and subscribing to
     * property change events.
     */
    class IInspectable {
    public:
        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~IInspectable() = default;

        /**
         * @brief Get the type name of this inspectable object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns a human-readable type name for display in the inspector
         * header (e.g., "Scene", "Character", "Item").
         *
         * @return std::string The type name
         */
        virtual std::string getTypeName() const = 0;

        /**
         * @brief Get the display name of this object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns a unique display name/identifier for this specific
         * instance (e.g., "Opening Scene", "Main Character").
         *
         * @return std::string The display name
         */
        virtual std::string getDisplayName() const = 0;

        /**
         * @brief Get all property descriptors for this object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns a list of all properties that can be inspected and
         * edited for this object. The descriptors contain metadata
         * about each property including type, constraints, and display info.
         *
         * @return std::vector<PropertyDescriptor> List of property descriptors
         */
        virtual std::vector<PropertyDescriptor> getPropertyDescriptors() const = 0;

        /**
         * @brief Get the value of a property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param propertyId The unique identifier of the property
         * @return PropertyValue The current value of the property
         *         Returns std::monostate if property not found
         */
        virtual PropertyValue getPropertyValue(const std::string& propertyId) const = 0;

        /**
         * @brief Set the value of a property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Sets a new value for the specified property. The implementation
         * should validate the value against any constraints and fire
         * property change events through the event dispatcher.
         *
         * @param propertyId The unique identifier of the property
         * @param value The new value to set
         * @return true if the value was set successfully
         * @return false if the property was not found or value was invalid
         */
        virtual bool setPropertyValue(
            const std::string& propertyId,
            const PropertyValue& value
        ) = 0;

        /**
         * @brief Get the event dispatcher for property changes
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the event dispatcher that can be used to subscribe
         * to property change events for this object.
         *
         * @return PropertyEventDispatcher& Reference to the event dispatcher
         */
        virtual PropertyEventDispatcher& getEventDispatcher() = 0;
    };
}

#endif //ADS_IINSPECTABLE_H