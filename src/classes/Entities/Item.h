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

#ifndef ADS_ITEM_ENTITY_H
#define ADS_ITEM_ENTITY_H

#include "BaseEntity.h"
#include "Data/ItemData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Item acts as the inspector adapter layer: it defines how item properties
     * are presented in the inspector UI, validates incoming values, and delegates
     * all persistent storage to the backing Data::ItemData struct owned by
     * Core::Project.
     */
    class Item : public BaseEntity {
    private:
        Data::ItemData* m_data; ///< Non-owning pointer to the backing ItemData

    public:
        /**
         * @brief Get the list of available item type names
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns a static list of human-readable type names (Generic, Key,
         * Weapon, etc.) used to populate the item type combo box.
         *
         * @return const std::vector<std::string>& Available item type names
         */
        static const std::vector<std::string>& getItemTypes();

        /**
         * @brief Construct a new Item backed by the given ItemData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the ItemData struct. Must not be
         *             null and must outlive this entity.
         */
        explicit Item(Data::ItemData* data);

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Always returns "Item"
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns metadata for all editable item properties, organised by
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

        // Item-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the item description shown to the player
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Item description text
         */
        const std::string& getDescription() const;

        /**
         * @brief Set the item description shown to the player
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
         * @brief Check whether the item can be picked up
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if the player can pick up this item
         */
        bool isPickable() const;

        /**
         * @brief Set whether the item can be picked up
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param pickable True to allow the player to pick up this item
         */
        void setPickable(bool pickable);

        /**
         * @brief Check whether the item can be used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if the player can use this item
         */
        bool isUsable() const;

        /**
         * @brief Set whether the item can be used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param usable True to allow the player to use this item
         */
        void setUsable(bool usable);

        /**
         * @brief Get the stack quantity of the item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Stack quantity
         */
        int getQuantity() const;

        /**
         * @brief Set the stack quantity of the item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param quantity The new quantity value
         */
        void setQuantity(int quantity);

        /**
         * @brief Get the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Index into the getItemTypes() list
         */
        int getItemType() const;

        /**
         * @brief Set the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event (as EnumValue) if the value actually changed.
         *
         * @param type Index into the getItemTypes() list
         */
        void setItemType(int type);

        /**
         * @brief Get the path to the item icon image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the icon image
         */
        const std::string& getIconPath() const;

        /**
         * @brief Set the path to the item icon image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new icon image file path
         */
        void setIconPath(const std::string& path);

        /**
         * @brief Get the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Starting scene ID
         */
        const std::string& getStartingSceneId() const;

        /**
         * @brief Set the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param sceneId The new starting scene ID
         */
        void setStartingSceneId(const std::string& sceneId);

        /**
         * @brief Get the item type name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Human-readable type name for the current item type index
         */
        std::string getItemTypeName() const;
    };
}

#endif //ADS_ITEM_ENTITY_H