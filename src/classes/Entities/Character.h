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

#ifndef ADS_CHARACTER_ENTITY_H
#define ADS_CHARACTER_ENTITY_H

#include "BaseEntity.h"
#include "Data/CharacterData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game character (player or NPC)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Character acts as the inspector adapter layer: it defines how character
     * properties are presented in the inspector UI, validates incoming values,
     * and delegates all persistent storage to the backing Data::CharacterData
     * struct owned by Core::Project.
     */
    class Character : public BaseEntity {
    private:
        Data::CharacterData* m_data; ///< Non-owning pointer to the backing CharacterData

    public:
        /**
         * @brief Construct a new Character backed by the given CharacterData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the CharacterData struct. Must not be
         *             null and must outlive this entity.
         */
        explicit Character(Data::CharacterData* data);

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Always returns "Character"
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns metadata for all editable character properties, organised by
         * category, for use by the inspector panel.
         *
         * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
         */
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;

        /**
         * @brief Get the current value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param propertyId The unique property identifier string
         * @return Inspector::PropertyValue Current value, or std::monostate if unknown
         */
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;

        /**
         * @brief Set the value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
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

        // Character-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the character's backstory and description text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Backstory and description text
         */
        const std::string& getDescription() const;

        /**
         * @brief Set the character's backstory and description text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param desc The new description text
         */
        void setDescription(const std::string& desc);

        /**
         * @brief Get the current health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Current health point value
         */
        int getHealth() const;

        /**
         * @brief Set the current health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param health The new health point value
         */
        void setHealth(int health);

        /**
         * @brief Get the maximum health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Maximum health point value
         */
        int getMaxHealth() const;

        /**
         * @brief Set the maximum health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param maxHealth The new maximum health point value
         */
        void setMaxHealth(int maxHealth);

        /**
         * @brief Check whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the player-controlled character
         */
        bool isPlayer() const;

        /**
         * @brief Set whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param isPlayer True to mark this as the player character
         */
        void setPlayer(bool isPlayer);

        /**
         * @brief Get the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Dialog text color
         */
        const ADS::Types::Color& getDialogColor() const;

        /**
         * @brief Set the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event (as ImVec4) if the value actually changed.
         *
         * @param color The new dialog text color
         */
        void setDialogColor(const ADS::Types::Color& color);

        /**
         * @brief Get the path to the character portrait image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the portrait image
         */
        const std::string& getPortraitPath() const;

        /**
         * @brief Set the path to the character portrait image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new portrait image file path
         */
        void setPortraitPath(const std::string& path);

        /**
         * @brief Get the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Starting scene ID
         */
        const std::string& getStartingSceneId() const;

        /**
         * @brief Set the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param sceneId The new starting scene ID
         */
        void setStartingSceneId(const std::string& sceneId);
    };
}

#endif //ADS_CHARACTER_ENTITY_H