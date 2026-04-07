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

#ifndef ADS_BASE_ENTITY_H
#define ADS_BASE_ENTITY_H

#include <string>
#include "Data/BaseData.h"
#include "Inspector/IInspectable.h"
#include "Inspector/PropertyEvent.h"

namespace ADS::Entities {
    /**
     * @brief Base class for all game entities (inspector adapter layer)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Provides common implementation for IInspectable interface,
     * including event dispatcher management and property change
     * notification helpers. Holds a non-owning pointer to a
     * Data::BaseData struct, which is the authoritative storage for
     * id and name. The DataObject is owned by Core::Project.
     */
    class BaseEntity : public Inspector::IInspectable {
    protected:
        Data::BaseData* m_baseData; ///< Non-owning pointer to the backing DataObject

        /// Event dispatcher for property changes
        Inspector::PropertyEventDispatcher m_eventDispatcher;

        /**
         * @brief Notify subscribers of a property change
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Helper method for derived classes to fire property change events.
         *
         * @param propertyId The ID of the changed property
         * @param oldValue The previous value
         * @param newValue The new value
         */
        void notifyPropertyChanged(
            const std::string& propertyId,
            const Inspector::PropertyValue& oldValue,
            const Inspector::PropertyValue& newValue
        );

    public:
        /**
         * @brief Construct a new BaseEntity backed by the given DataObject
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the BaseData struct. Must not be null
         *             and must outlive this entity (Core::Project guarantees this).
         */
        explicit BaseEntity(Data::BaseData* data);

        /**
         * @brief Virtual destructor
         */
        ~BaseEntity() override = default;

        // IInspectable interface

        /**
         * @brief Get the display name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Reads the name from the backing DataObject.
         *
         * @return std::string Human-readable display name
         */
        std::string getDisplayName() const override;

        /**
         * @brief Get the property event dispatcher
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns a reference to the dispatcher used to subscribe to and
         * fire property-changed events for this entity.
         *
         * @return Inspector::PropertyEventDispatcher& Reference to the event dispatcher
         */
        Inspector::PropertyEventDispatcher& getEventDispatcher() override;

        /**
         * @brief Get the unique identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Entity ID read from the backing DataObject
         */
        const std::string& getId() const;

        /**
         * @brief Set the display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Writes the new name into the backing DataObject and fires a
         * property-changed event if the value actually changed.
         *
         * @param name New display name
         */
        void setName(const std::string& name);

    protected:
        /**
         * @brief Set a DataObject field and fire a property-changed event
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Reads the current value via @p getter, compares it to @p newValue,
         * and only writes and notifies if the value actually changed. Handles
         * all scalar and string properties (bool, int, std::string). Color
         * properties require manual handling due to the ADS::Types::Color ↔
         * ImVec4 conversion needed for notifyPropertyChanged.
         *
         * @tparam T        Property value type (deduced from newValue)
         * @tparam Getter   Callable returning T — reads from the DataObject
         * @tparam Setter   Callable taking const T& — writes to the DataObject
         * @param propertyId String key used for the change event
         * @param getter     Lambda that returns the current value
         * @param setter     Lambda that applies the new value
         * @param newValue   The value to set
         */
        template<typename T, typename Getter, typename Setter>
        void setAndNotify(const std::string& propertyId,
                          Getter getter, Setter setter, const T& newValue)
        {
            T current = getter();
            if (current != newValue) {
                setter(newValue);
                notifyPropertyChanged(propertyId, current, newValue);
            }
        }
    };
}

#endif //ADS_BASE_ENTITY_H
