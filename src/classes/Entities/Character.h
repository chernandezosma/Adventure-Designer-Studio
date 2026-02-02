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

#ifndef ADS_CHARACTER_ENTITY_H
#define ADS_CHARACTER_ENTITY_H

#include "BaseEntity.h"
#include "imgui.h"

namespace ADS::Entities {
    /**
     * @brief Character entity representing a game character (player, NPC, etc.)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * A character can be the player, an NPC, or any other interactive
     * character in the game world.
     */
    class Character : public BaseEntity {
    private:
        std::string m_description;      ///< Character description/backstory
        int m_health;                   ///< Current health points
        int m_maxHealth;                ///< Maximum health points
        bool m_isPlayer;                ///< Whether this is the player character
        ImVec4 m_dialogColor;           ///< Color used for dialog text

    public:
        /**
         * @brief Construct a new Character
         *
         * @param id Unique identifier
         * @param name Display name
         */
        Character(const std::string& id, const std::string& name);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Character-specific getters/setters
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        int getHealth() const;
        void setHealth(int health);

        int getMaxHealth() const;
        void setMaxHealth(int maxHealth);

        bool isPlayer() const;
        void setPlayer(bool isPlayer);

        const ImVec4& getDialogColor() const;
        void setDialogColor(const ImVec4& color);
    };
}

#endif //ADS_CHARACTER_ENTITY_H
