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

    Item::Item(Data::ItemData* data)
        : BaseEntity(data), m_data(data) {
    }

    std::string Item::getTypeName() const {
        return "Item";
    }

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

    Inspector::PropertyValue Item::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name")        return m_data->name;
        if (propertyId == "description") return m_data->description;
        if (propertyId == "isPickable")  return m_data->isPickable;
        if (propertyId == "isUsable")    return m_data->isUsable;
        if (propertyId == "quantity")    return m_data->quantity;
        if (propertyId == "id")          return m_data->id;
        if (propertyId == "iconPath")         return m_data->iconPath;
        if (propertyId == "startingSceneId")  return m_data->startingSceneId;
        if (propertyId == "itemType") {
            return Inspector::EnumValue(m_data->itemType, getItemTypes());
        }

        return std::monostate{};
    }

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

    const std::string& Item::getDescription() const {
        return m_data->description;
    }

    void Item::setDescription(const std::string& desc) {
        if (m_data->description != desc) {
            std::string oldDesc = m_data->description;
            m_data->description = desc;
            notifyPropertyChanged("description", oldDesc, m_data->description);
        }
    }

    bool Item::isPickable() const {
        return m_data->isPickable;
    }

    void Item::setPickable(bool pickable) {
        if (m_data->isPickable != pickable) {
            bool oldValue = m_data->isPickable;
            m_data->isPickable = pickable;
            notifyPropertyChanged("isPickable", oldValue, m_data->isPickable);
        }
    }

    bool Item::isUsable() const {
        return m_data->isUsable;
    }

    void Item::setUsable(bool usable) {
        if (m_data->isUsable != usable) {
            bool oldValue = m_data->isUsable;
            m_data->isUsable = usable;
            notifyPropertyChanged("isUsable", oldValue, m_data->isUsable);
        }
    }

    int Item::getQuantity() const {
        return m_data->quantity;
    }

    void Item::setQuantity(int quantity) {
        if (m_data->quantity != quantity) {
            int oldQuantity = m_data->quantity;
            m_data->quantity = quantity;
            notifyPropertyChanged("quantity", oldQuantity, m_data->quantity);
        }
    }

    int Item::getItemType() const {
        return m_data->itemType;
    }

    void Item::setItemType(int type) {
        if (m_data->itemType != type) {
            int oldType = m_data->itemType;
            m_data->itemType = type;
            notifyPropertyChanged("itemType",
                Inspector::EnumValue(oldType, getItemTypes()),
                Inspector::EnumValue(m_data->itemType, getItemTypes()));
        }
    }

    const std::string& Item::getIconPath() const {
        return m_data->iconPath;
    }

    void Item::setIconPath(const std::string& path) {
        if (m_data->iconPath != path) {
            std::string oldPath = m_data->iconPath;
            m_data->iconPath = path;
            notifyPropertyChanged("iconPath", oldPath, m_data->iconPath);
        }
    }

    const std::string& Item::getStartingSceneId() const {
        return m_data->startingSceneId;
    }

    void Item::setStartingSceneId(const std::string& sceneId) {
        if (m_data->startingSceneId != sceneId) {
            std::string oldId = m_data->startingSceneId;
            m_data->startingSceneId = sceneId;
            notifyPropertyChanged("startingSceneId", oldId, m_data->startingSceneId);
        }
    }

    std::string Item::getItemTypeName() const {
        const auto& types = getItemTypes();
        if (m_data->itemType >= 0 && m_data->itemType < static_cast<int>(types.size())) {
            return types[m_data->itemType];
        }
        return "Unknown";
    }
}
