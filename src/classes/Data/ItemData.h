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

#ifndef ADS_DATA_ITEM_DATA_H
#define ADS_DATA_ITEM_DATA_H

/**
 * @file ItemData.h
 * @brief Pure data class for a game item — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>

#include "BaseData.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a game item.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Holds all persistent data for an item. Only standard C++ types are
     * used — no ImGui or UI framework types appear here.
     * This class is owned by Core::Project; Entities::Item reads/writes
     * through a non-owning pointer to this object.
     */
    class ItemData : public BaseData {
    public:
        /**
         * @brief Get the item description shown to the player
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Item description text
         */
        [[nodiscard]] const std::string& getDescription() const { return m_description; }

        /**
         * @brief Set the item description shown to the player
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param description The new description text
         */
        void setDescription(const std::string& description) { m_description = description; }

        /**
         * @brief Get the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Index into the type list (Generic, Key, Weapon, etc.).
         *
         * @return int Item type index
         */
        [[nodiscard]] int getItemType() const { return m_itemType; }

        /**
         * @brief Set the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param itemType The new item type index
         */
        void setItemType(int itemType) { m_itemType = itemType; }

        /**
         * @brief Check whether the item can be picked up
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if the player can pick up this item
         */
        [[nodiscard]] bool isPickable() const { return m_isPickable; }

        /**
         * @brief Set whether the item can be picked up
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param pickable True to allow the player to pick up this item
         */
        void setPickable(bool pickable) { m_isPickable = pickable; }

        /**
         * @brief Check whether the item can be used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if the player can use this item
         */
        [[nodiscard]] bool isUsable() const { return m_isUsable; }

        /**
         * @brief Set whether the item can be used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param usable True to allow the player to use this item
         */
        void setUsable(bool usable) { m_isUsable = usable; }

        /**
         * @brief Get the stack quantity of the item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Stack quantity
         */
        [[nodiscard]] int getQuantity() const { return m_quantity; }

        /**
         * @brief Set the stack quantity of the item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param quantity The new quantity value
         */
        void setQuantity(int quantity) { m_quantity = quantity; }

        /**
         * @brief Get the path to the item icon image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the icon image
         */
        [[nodiscard]] const std::string& getIconPath() const { return m_iconPath; }

        /**
         * @brief Set the path to the item icon image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param path The new icon image file path
         */
        void setIconPath(const std::string& path) { m_iconPath = path; }

        /**
         * @brief Get the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Starting scene ID
         */
        [[nodiscard]] const std::string& getStartingSceneId() const { return m_startingSceneId; }

        /**
         * @brief Set the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sceneId The new starting scene ID
         */
        void setStartingSceneId(const std::string& sceneId) { m_startingSceneId = sceneId; }

    private:
        std::string m_description;                  ///< Item description shown to the player
        int         m_itemType      = 0;             ///< Item type index (Generic, Key, Weapon, etc.)
        bool        m_isPickable    = true;          ///< Whether the item can be picked up
        bool        m_isUsable      = false;         ///< Whether the item can be used
        int         m_quantity      = 1;             ///< Stack quantity
        std::string m_iconPath;                      ///< Path to icon image asset
        std::string m_startingSceneId;               ///< ID of the scene where this item starts
    };

} // namespace ADS::Data

#endif // ADS_DATA_ITEM_DATA_H