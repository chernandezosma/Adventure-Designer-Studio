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

#ifndef ADS_STATE_ENTITY_H
#define ADS_STATE_ENTITY_H

#include "BaseEntity.h"
#include "Data/StateData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a state-catalog entry
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * State acts as the inspector adapter layer: it defines how a state's
     * properties are presented in the inspector UI, validates incoming
     * values, and delegates all persistent storage to the backing
     * Data::StateData struct owned by Core::Project. States are linked into
     * chains via next — a nullable single-select over every other state in
     * the project, resolved live from BaseEntity::getProject() the same way
     * Scene resolves its exit directions.
     */
    class State : public BaseEntity {
    private:
        Data::StateData* m_data; ///< Non-owning pointer to the backing StateData

        /**
         * @brief Build the option-label list for every state in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Used as the options provider for the next property. Returns an
         * empty list if getProject() is unset.
         *
         * @return std::vector<std::string> One label per state, in project order
         */
        std::vector<std::string> buildStateOptionLabels() const;

    public:
        /**
         * @brief Construct a new State backed by the given StateData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param data Non-owning pointer to the StateData struct. Must not
         *             be null and must outlive this entity.
         */
        explicit State(Data::StateData* data);

        /**
         * @brief Get the typed state identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Distinct from BaseEntity::getId() (tag-erased display string) —
         * returns the underlying ADS::Types::StateId for code that needs
         * the typed id.
         *
         * @return ADS::Types::StateId The state's typed identifier
         */
        ADS::Types::StateId getStateId() const;

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return std::string Translated "State" type name
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this state
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

        // State-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the sensory descriptions shown while this state is active
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Not exposed as an Inspector property — see descriptionsNormal etc.
         *
         * @return const Data::Descriptions& LexEngine text-id references for this state
         */
        const Data::Descriptions& getDescriptions() const;

        /**
         * @brief Set the sensory descriptions shown while this state is active
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Data::Descriptions& descriptions);

        /**
         * @brief Get the author-typed draft text for this state's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const Data::DescriptionTexts& Draft description text
         */
        const Data::DescriptionTexts& getDescriptionTexts() const;

        /**
         * @brief Set the author-typed draft text for this state's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const Data::DescriptionTexts& texts);

        /**
         * @brief Get the next state in this state's chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Next state id, or
         *         std::nullopt if this is the last state in its chain
         */
        const std::optional<ADS::Types::StateId>& getNext() const;

        /**
         * @brief Set the next state in this state's chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param next The new next-state id, or std::nullopt to mark this as
         *             the last state in its chain
         */
        void setNext(const std::optional<ADS::Types::StateId>& next);
    };
}

#endif //ADS_STATE_ENTITY_H
