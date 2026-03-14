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

/**
 * @file Item.cpp
 * @brief Implementation of the Item entity class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
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

    Item::Item(const std::string& id, const std::string& name)
        : BaseEntity(id, name),
          m_isPickable(true),
          m_isUsable(false),
          m_quantity(1),
          m_itemType(0) {
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

            // Info category (read-only)
            PropertyDescriptor("id", "ID", PropertyType::String)
                .setCategory("Info")
                .setDescription("Unique item identifier")
                .setReadOnly()
        };
    }

    Inspector::PropertyValue Item::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name") return m_name;
        if (propertyId == "description") return m_description;
        if (propertyId == "isPickable") return m_isPickable;
        if (propertyId == "isUsable") return m_isUsable;
        if (propertyId == "quantity") return m_quantity;
        if (propertyId == "id") return m_id;
        if (propertyId == "itemType") {
            return Inspector::EnumValue(m_itemType, getItemTypes());
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

        return false;
    }

    const std::string& Item::getDescription() const {
        return m_description;
    }

    void Item::setDescription(const std::string& desc) {
        if (m_description != desc) {
            std::string oldDesc = m_description;
            m_description = desc;
            notifyPropertyChanged("description", oldDesc, m_description);
        }
    }

    bool Item::isPickable() const {
        return m_isPickable;
    }

    void Item::setPickable(bool pickable) {
        if (m_isPickable != pickable) {
            bool oldValue = m_isPickable;
            m_isPickable = pickable;
            notifyPropertyChanged("isPickable", oldValue, m_isPickable);
        }
    }

    bool Item::isUsable() const {
        return m_isUsable;
    }

    void Item::setUsable(bool usable) {
        if (m_isUsable != usable) {
            bool oldValue = m_isUsable;
            m_isUsable = usable;
            notifyPropertyChanged("isUsable", oldValue, m_isUsable);
        }
    }

    int Item::getQuantity() const {
        return m_quantity;
    }

    void Item::setQuantity(int quantity) {
        if (m_quantity != quantity) {
            int oldQuantity = m_quantity;
            m_quantity = quantity;
            notifyPropertyChanged("quantity", oldQuantity, m_quantity);
        }
    }

    int Item::getItemType() const {
        return m_itemType;
    }

    void Item::setItemType(int type) {
        if (m_itemType != type) {
            int oldType = m_itemType;
            m_itemType = type;
            notifyPropertyChanged("itemType",
                Inspector::EnumValue(oldType, getItemTypes()),
                Inspector::EnumValue(m_itemType, getItemTypes()));
        }
    }

    std::string Item::getItemTypeName() const {
        const auto& types = getItemTypes();
        if (m_itemType >= 0 && m_itemType < static_cast<int>(types.size())) {
            return types[m_itemType];
        }
        return "Unknown";
    }
}
