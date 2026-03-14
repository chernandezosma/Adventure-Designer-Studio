/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */

#ifndef ADS_BASE_ENTITY_H
#define ADS_BASE_ENTITY_H

#include <string>
#include "Inspector/IInspectable.h"
#include "Inspector/PropertyEvent.h"

namespace ADS::Entities {
    /**
     * @brief Base class for all game entities
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides common implementation for IInspectable interface,
     * including event dispatcher management and property change
     * notification helpers.
     */
    class BaseEntity : public Inspector::IInspectable {
    protected:
        std::string m_id;       ///< Unique entity identifier
        std::string m_name;     ///< Display name

        /// Event dispatcher for property changes
        Inspector::PropertyEventDispatcher m_eventDispatcher;

        /**
         * @brief Notify subscribers of a property change
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
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
         * @brief Construct a new BaseEntity
         *
         * @param id Unique identifier
         * @param name Display name
         */
        BaseEntity(const std::string& id, const std::string& name);

        /**
         * @brief Virtual destructor
         */
        ~BaseEntity() override = default;

        // IInspectable interface
        std::string getDisplayName() const override;
        Inspector::PropertyEventDispatcher& getEventDispatcher() override;

        /**
         * @brief Get the unique identifier
         * @return const std::string& Entity ID
         */
        const std::string& getId() const;

        /**
         * @brief Set the display name
         * @param name New name
         */
        void setName(const std::string& name);
    };
}

#endif //ADS_BASE_ENTITY_H
