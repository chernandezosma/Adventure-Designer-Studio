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
        return m_data->getDescription();
    }

    void Item::setDescription(const std::string& desc) {
        setAndNotify("description",
            [this]{ return m_data->getDescription(); },
            [this](const std::string& v){ m_data->setDescription(v); },
            desc);
    }

    bool Item::isPickable() const {
        return m_data->isPickable();
    }

    void Item::setPickable(bool pickable) {
        setAndNotify("isPickable",
            [this]{ return m_data->isPickable(); },
            [this](bool v){ m_data->setPickable(v); },
            pickable);
    }

    bool Item::isUsable() const {
        return m_data->isUsable();
    }

    void Item::setUsable(bool usable) {
        setAndNotify("isUsable",
            [this]{ return m_data->isUsable(); },
            [this](bool v){ m_data->setUsable(v); },
            usable);
    }

    int Item::getQuantity() const {
        return m_data->getQuantity();
    }

    void Item::setQuantity(int quantity) {
        setAndNotify("quantity",
            [this]{ return m_data->getQuantity(); },
            [this](int v){ m_data->setQuantity(v); },
            quantity);
    }

    int Item::getItemType() const {
        return m_data->getItemType();
    }

    void Item::setItemType(int type) {
        if (m_data->getItemType() != type) {
            int oldType = m_data->getItemType();
            m_data->setItemType(type);
            notifyPropertyChanged("itemType",
                Inspector::EnumValue(oldType, getItemTypes()),
                Inspector::EnumValue(m_data->getItemType(), getItemTypes()));
        }
    }

    const std::string& Item::getIconPath() const {
        return m_data->getIconPath();
    }

    void Item::setIconPath(const std::string& path) {
        setAndNotify("iconPath",
            [this]{ return m_data->getIconPath(); },
            [this](const std::string& v){ m_data->setIconPath(v); },
            path);
    }

    const std::string& Item::getStartingSceneId() const {
        return m_data->getStartingSceneId();
    }

    void Item::setStartingSceneId(const std::string& sceneId) {
        setAndNotify("startingSceneId",
            [this]{ return m_data->getStartingSceneId(); },
            [this](const std::string& v){ m_data->setStartingSceneId(v); },
            sceneId);
    }

    std::string Item::getItemTypeName() const {
        const auto& types = getItemTypes();
        const int type = m_data->getItemType();
        if (type >= 0 && type < static_cast<int>(types.size())) {
            return types[type];
        }
        return "Unknown";
    }
}