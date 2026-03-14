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

#ifndef ADS_ITEM_ENTITY_H
#define ADS_ITEM_ENTITY_H

#include "BaseEntity.h"

namespace ADS::Entities {
    /**
     * @brief Item entity representing a game object/item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * An item is an interactive object in the game world that can be
     * picked up, used, or interacted with by the player.
     */
    class Item : public BaseEntity {
    private:
        std::string m_description;      ///< Item description
        bool m_isPickable;              ///< Whether the item can be picked up
        bool m_isUsable;                ///< Whether the item can be used
        int m_quantity;                 ///< Stack quantity
        int m_itemType;                 ///< Item type index (Key, Weapon, Consumable, etc.)

    public:
        /// Available item types
        static const std::vector<std::string>& getItemTypes();

        /**
         * @brief Construct a new Item
         *
         * @param id Unique identifier
         * @param name Display name
         */
        Item(const std::string& id, const std::string& name);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Item-specific getters/setters
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        bool isPickable() const;
        void setPickable(bool pickable);

        bool isUsable() const;
        void setUsable(bool usable);

        int getQuantity() const;
        void setQuantity(int quantity);

        int getItemType() const;
        void setItemType(int type);

        /**
         * @brief Get the item type name
         * @return std::string Human-readable type name
         */
        std::string getItemTypeName() const;
    };
}

#endif //ADS_ITEM_ENTITY_H
