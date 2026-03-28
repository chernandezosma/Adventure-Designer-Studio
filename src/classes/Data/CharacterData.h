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

#ifndef ADS_DATA_CHARACTER_DATA_H
#define ADS_DATA_CHARACTER_DATA_H

/**
 * @file CharacterData.h
 * @brief Pure data class for a game character — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>
#include <vector>

#include "BaseData.h"
#include "Types/Color.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a game character (player or NPC).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Holds all persistent data for a character. Only standard C++ types and
     * ADS::Types are used — no ImGui or UI framework types appear here.
     * This class is owned by Core::Project; Entities::Character reads/writes
     * through a non-owning pointer to this object.
     */
    class CharacterData : public BaseData {
    public:
        /**
         * @brief Get the character's backstory and description text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Backstory and description text
         */
        [[nodiscard]] const std::string& getDescription() const { return m_description; }

        /**
         * @brief Set the character's backstory and description text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param description The new description text
         */
        void setDescription(const std::string& description) { m_description = description; }

        /**
         * @brief Check whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the player-controlled character
         */
        [[nodiscard]] bool isPlayer() const { return m_isPlayer; }

        /**
         * @brief Set whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param isPlayer True to mark this as the player character
         */
        void setPlayer(bool isPlayer) { m_isPlayer = isPlayer; }

        /**
         * @brief Get the current health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Current health point value
         */
        [[nodiscard]] int getHealth() const { return m_health; }

        /**
         * @brief Set the current health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param health The new health point value
         */
        void setHealth(int health) { m_health = health; }

        /**
         * @brief Get the maximum health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Maximum health point value
         */
        [[nodiscard]] int getMaxHealth() const { return m_maxHealth; }

        /**
         * @brief Set the maximum health points
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param maxHealth The new maximum health point value
         */
        void setMaxHealth(int maxHealth) { m_maxHealth = maxHealth; }

        /**
         * @brief Get the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Dialog text color
         */
        [[nodiscard]] const ADS::Types::Color& getDialogColor() const { return m_dialogColor; }

        /**
         * @brief Set the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param color The new dialog text color
         */
        void setDialogColor(const ADS::Types::Color& color) { m_dialogColor = color; }

        /**
         * @brief Get the path to the character portrait image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the portrait image
         */
        [[nodiscard]] const std::string& getPortraitPath() const { return m_portraitPath; }

        /**
         * @brief Set the path to the character portrait image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param path The new portrait image file path
         */
        void setPortraitPath(const std::string& path) { m_portraitPath = path; }

        /**
         * @brief Get the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Starting scene ID
         */
        [[nodiscard]] const std::string& getStartingSceneId() const { return m_startingSceneId; }

        /**
         * @brief Set the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sceneId The new starting scene ID
         */
        void setStartingSceneId(const std::string& sceneId) { m_startingSceneId = sceneId; }

        /**
         * @brief Get the IDs of items initially in this character's inventory
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::string>& List of inventory item IDs
         */
        [[nodiscard]] const std::vector<std::string>& getInventoryItemIds() const { return m_inventoryItemIds; }

        /**
         * @brief Set the IDs of items initially in this character's inventory
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param ids The new inventory item ID vector
         */
        void setInventoryItemIds(const std::vector<std::string>& ids) { m_inventoryItemIds = ids; }

    private:
        std::string       m_description;                                        ///< Backstory and description text
        bool              m_isPlayer   = false;                                 ///< Whether this is the player character
        int               m_health     = 100;                                   ///< Current health points
        int               m_maxHealth  = 100;                                   ///< Maximum health points
        ADS::Types::Color m_dialogColor = {1.0f, 1.0f, 1.0f, 1.0f};           ///< Color used for dialog text
        std::string       m_portraitPath;                                       ///< Path to character portrait image
        std::string       m_startingSceneId;                                    ///< ID of the scene where this character starts
        std::vector<std::string> m_inventoryItemIds;                            ///< IDs of items initially in inventory
    };

} // namespace ADS::Data

#endif // ADS_DATA_CHARACTER_DATA_H