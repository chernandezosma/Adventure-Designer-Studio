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

#ifndef ADS_STATE_CHAIN_ENTITY_H
#define ADS_STATE_CHAIN_ENTITY_H

#include "BaseEntity.h"
#include "Data/StateChainData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a named state chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * StateChain acts as the inspector adapter layer: it defines how a
     * chain's properties are presented in the inspector UI, validates
     * incoming values, and delegates all persistent storage to the backing
     * Data::StateChainData struct owned by Core::Project. Other entities
     * (Scene today) reference a chain by id rather than a state directly —
     * see buildChainOptionLabels(), the options provider consumed by
     * Scene's "state" property.
     */
    class StateChain : public BaseEntity {
    private:
        Data::StateChainData* m_data; ///< Non-owning pointer to the backing StateChainData

        /**
         * @brief Build the option-label list for every state in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Used as the options provider for the head property. Returns an
         * empty list if getProject() is unset.
         *
         * @return std::vector<std::string> One label per state, in project order
         */
        std::vector<std::string> buildStateOptionLabels() const;

    public:
        /**
         * @brief Construct a new StateChain backed by the given StateChainData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param data Non-owning pointer to the StateChainData struct. Must
         *             not be null and must outlive this entity.
         */
        explicit StateChain(Data::StateChainData* data);

        /**
         * @brief Get the typed chain identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Distinct from BaseEntity::getId() (tag-erased display string) —
         * returns the underlying ADS::Types::ChainId for code that needs
         * the typed id.
         *
         * @return ADS::Types::ChainId The chain's typed identifier
         */
        ADS::Types::ChainId getChainId() const;

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return std::string Translated "Chain" type name
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
         */
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;

        /**
         * @brief Get the current value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param propertyId The unique property identifier string
         * @return Inspector::PropertyValue Current value, or std::monostate if unknown
         */
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;

        /**
         * @brief Set the value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Validates the type of @p value against the expected type for
         * @p propertyId before writing. Fires a property-changed event on success.
         *
         * @param propertyId The unique property identifier string
         * @param value The new value (must match the property's expected type)
         * @return bool True if the value was accepted and written, false otherwise
         */
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // StateChain-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the head state of this chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Head state id,
         *         or std::nullopt if this chain has no states yet
         */
        const std::optional<ADS::Types::StateId>& getHead() const;

        /**
         * @brief Set the head state of this chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param head The new head state id, or std::nullopt to clear it
         */
        void setHead(const std::optional<ADS::Types::StateId>& head);
    };
}

#endif //ADS_STATE_CHAIN_ENTITY_H
