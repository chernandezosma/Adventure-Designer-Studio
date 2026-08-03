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

/**
 * @file Item.cpp
 * @brief Implementation of the Item entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "Item.h"

namespace ADS::Entities {
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
    const std::vector<std::string>& Item::getItemTypes() {
        static const std::vector<std::string> types = {
            "Generic",
            "Key",
            "Weapon",
            "Armor",
            "Consumable",
            "Quest Item",
            "Document",
            "Container"
        };
        return types;
    }

    /**
     * @brief Construct a new Item backed by the given ItemData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param data Non-owning pointer to the ItemData struct. Must not be
     *             null and must outlive this entity.
     */
    Item::Item(Data::ItemData* data)
        : BaseEntity(data), m_data(data) {
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Always returns "Item"
     */
    std::string Item::getTypeName() const {
        return "Item";
    }

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
    std::vector<Inspector::PropertyDescriptor> Item::getPropertyDescriptors() const {
        using namespace Inspector;

        return {
            // General category
            PropertyDescriptor("name", "Name", PropertyType::String)
                .setCategory("General")
                .setDescription("Item display name")
                .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("description", "Description", PropertyType::String)
                .setCategory("General")
                .setDescription("Item description shown to the player"),

            PropertyDescriptor("itemType", "Type", PropertyType::Enum)
                .setCategory("General")
                .setDescription("The type/category of item")
                .setConstraints(PropertyConstraints::enumeration(getItemTypes())),

            PropertyDescriptor("startingSceneId", "Starting Scene", PropertyType::String)
                .setCategory("General")
                .setDescription("ID of the scene where this item starts"),

            // Behavior category
            PropertyDescriptor("isPickable", "Pickable", PropertyType::Bool)
                .setCategory("Behavior")
                .setDescription("Can the player pick up this item?"),

            PropertyDescriptor("isUsable", "Usable", PropertyType::Bool)
                .setCategory("Behavior")
                .setDescription("Can the player use this item?"),

            PropertyDescriptor("quantity", "Quantity", PropertyType::Int)
                .setCategory("Behavior")
                .setDescription("Stack quantity")
                .setConstraints(PropertyConstraints::numeric(1, 999, 1)),

            // Appearance category
            PropertyDescriptor("iconPath", "Icon", PropertyType::String)
                .setCategory("Appearance")
                .setDescription("Path to the item icon image"),

            // Info category (read-only)
            PropertyDescriptor("id", "ID", PropertyType::String)
                .setCategory("Info")
                .setDescription("Unique item identifier")
                .setReadOnly()
        };
    }

    /**
     * @brief Get the current value of a property by ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param propertyId The unique property identifier string
     * @return Inspector::PropertyValue Current value, or std::monostate if unknown
     */
    Inspector::PropertyValue Item::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name")        return m_data->getName();
        if (propertyId == "description") return m_data->getDescription();
        if (propertyId == "isPickable")  return m_data->isPickable();
        if (propertyId == "isUsable")    return m_data->isUsable();
        if (propertyId == "quantity")    return m_data->getQuantity();
        if (propertyId == "id")          return m_data->getId();
        if (propertyId == "iconPath")         return m_data->getIconPath();
        if (propertyId == "startingSceneId")  return m_data->getStartingSceneId();
        if (propertyId == "itemType") {
            return Inspector::EnumValue(m_data->getItemType(), getItemTypes());
        }

        return std::monostate{};
    }

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
    bool Item::setPropertyValue(
        const std::string& propertyId,
        const Inspector::PropertyValue& value
    ) {
        if (propertyId == "name") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setName(*str);
                return true;
            }
        }
        else if (propertyId == "description") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setDescription(*str);
                return true;
            }
        }
        else if (propertyId == "isPickable") {
            if (auto* b = std::get_if<bool>(&value)) {
                setPickable(*b);
                return true;
            }
        }
        else if (propertyId == "isUsable") {
            if (auto* b = std::get_if<bool>(&value)) {
                setUsable(*b);
                return true;
            }
        }
        else if (propertyId == "quantity") {
            if (auto* i = std::get_if<int>(&value)) {
                setQuantity(*i);
                return true;
            }
        }
        else if (propertyId == "itemType") {
            if (auto* ev = std::get_if<Inspector::EnumValue>(&value)) {
                setItemType(ev->selectedIndex);
                return true;
            }
        }
        else if (propertyId == "iconPath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setIconPath(*str);
                return true;
            }
        }
        else if (propertyId == "startingSceneId") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setStartingSceneId(*str);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Get the item description shown to the player
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::string& Item description text
     */
    const std::string& Item::getDescription() const {
        return m_data->getDescription();
    }

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
    void Item::setDescription(const std::string& desc) {
        setAndNotify("description",
            [this]{ return m_data->getDescription(); },
            [this](const std::string& v){ m_data->setDescription(v); },
            desc);
    }

    /**
     * @brief Check whether the item can be picked up
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return bool True if the player can pick up this item
     */
    bool Item::isPickable() const {
        return m_data->isPickable();
    }

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
    void Item::setPickable(bool pickable) {
        setAndNotify("isPickable",
            [this]{ return m_data->isPickable(); },
            [this](bool v){ m_data->setPickable(v); },
            pickable);
    }

    /**
     * @brief Check whether the item can be used
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return bool True if the player can use this item
     */
    bool Item::isUsable() const {
        return m_data->isUsable();
    }

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
    void Item::setUsable(bool usable) {
        setAndNotify("isUsable",
            [this]{ return m_data->isUsable(); },
            [this](bool v){ m_data->setUsable(v); },
            usable);
    }

    /**
     * @brief Get the stack quantity of the item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return int Stack quantity
     */
    int Item::getQuantity() const {
        return m_data->getQuantity();
    }

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
    void Item::setQuantity(int quantity) {
        setAndNotify("quantity",
            [this]{ return m_data->getQuantity(); },
            [this](int v){ m_data->setQuantity(v); },
            quantity);
    }

    /**
     * @brief Get the item type index
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return int Index into the getItemTypes() list
     */
    int Item::getItemType() const {
        return m_data->getItemType();
    }

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
    void Item::setItemType(int type) {
        if (m_data->getItemType() != type) {
            int oldType = m_data->getItemType();
            m_data->setItemType(type);
            notifyPropertyChanged("itemType",
                Inspector::EnumValue(oldType, getItemTypes()),
                Inspector::EnumValue(m_data->getItemType(), getItemTypes()));
        }
    }

    /**
     * @brief Get the path to the item icon image
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::string& File path to the icon image
     */
    const std::string& Item::getIconPath() const {
        return m_data->getIconPath();
    }

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
    void Item::setIconPath(const std::string& path) {
        setAndNotify("iconPath",
            [this]{ return m_data->getIconPath(); },
            [this](const std::string& v){ m_data->setIconPath(v); },
            path);
    }

    /**
     * @brief Get the ID of the scene where this item starts
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::string& Starting scene ID
     */
    const std::string& Item::getStartingSceneId() const {
        return m_data->getStartingSceneId();
    }

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
    void Item::setStartingSceneId(const std::string& sceneId) {
        setAndNotify("startingSceneId",
            [this]{ return m_data->getStartingSceneId(); },
            [this](const std::string& v){ m_data->setStartingSceneId(v); },
            sceneId);
    }

    /**
     * @brief Get the item type name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Human-readable type name for the current item type index
     */
    std::string Item::getItemTypeName() const {
        const auto& types = getItemTypes();
        const int type = m_data->getItemType();
        if (type >= 0 && type < static_cast<int>(types.size())) {
            return types[type];
        }
        return "Unknown";
    }
}