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
 * @file Character.cpp
 * @brief Implementation of the Character entity class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "Character.h"

namespace ADS::Entities {
    Character::Character(const std::string& id, const std::string& name)
        : BaseEntity(id, name),
          m_health(100),
          m_maxHealth(100),
          m_isPlayer(false),
          m_dialogColor(1.0f, 1.0f, 1.0f, 1.0f) {
    }

    std::string Character::getTypeName() const {
        return "Character";
    }

    std::vector<Inspector::PropertyDescriptor> Character::getPropertyDescriptors() const {
        using namespace Inspector;

        return {
            // General category
            PropertyDescriptor("name", "Name", PropertyType::String)
                .setCategory("General")
                .setDescription("Character's display name")
                .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("description", "Description", PropertyType::String)
                .setCategory("General")
                .setDescription("Character backstory and description"),

            PropertyDescriptor("isPlayer", "Player Character", PropertyType::Bool)
                .setCategory("General")
                .setDescription("Set as the player-controlled character"),

            // Stats category
            PropertyDescriptor("health", "Health", PropertyType::Int)
                .setCategory("Stats")
                .setDescription("Current health points")
                .setConstraints(PropertyConstraints::numeric(0, 9999, 1)),

            PropertyDescriptor("maxHealth", "Max Health", PropertyType::Int)
                .setCategory("Stats")
                .setDescription("Maximum health points")
                .setConstraints(PropertyConstraints::numeric(1, 9999, 1)),

            // Appearance category
            PropertyDescriptor("dialogColor", "Dialog Color", PropertyType::Color)
                .setCategory("Appearance")
                .setDescription("Color used for this character's dialog text"),

            // Info category (read-only)
            PropertyDescriptor("id", "ID", PropertyType::String)
                .setCategory("Info")
                .setDescription("Unique character identifier")
                .setReadOnly()
        };
    }

    Inspector::PropertyValue Character::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name") return m_name;
        if (propertyId == "description") return m_description;
        if (propertyId == "isPlayer") return m_isPlayer;
        if (propertyId == "health") return m_health;
        if (propertyId == "maxHealth") return m_maxHealth;
        if (propertyId == "dialogColor") return m_dialogColor;
        if (propertyId == "id") return m_id;

        return std::monostate{};
    }

    bool Character::setPropertyValue(
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
        else if (propertyId == "isPlayer") {
            if (auto* b = std::get_if<bool>(&value)) {
                setPlayer(*b);
                return true;
            }
        }
        else if (propertyId == "health") {
            if (auto* i = std::get_if<int>(&value)) {
                setHealth(*i);
                return true;
            }
        }
        else if (propertyId == "maxHealth") {
            if (auto* i = std::get_if<int>(&value)) {
                setMaxHealth(*i);
                return true;
            }
        }
        else if (propertyId == "dialogColor") {
            if (auto* color = std::get_if<ImVec4>(&value)) {
                setDialogColor(*color);
                return true;
            }
        }

        return false;
    }

    const std::string& Character::getDescription() const {
        return m_description;
    }

    void Character::setDescription(const std::string& desc) {
        if (m_description != desc) {
            std::string oldDesc = m_description;
            m_description = desc;
            notifyPropertyChanged("description", oldDesc, m_description);
        }
    }

    int Character::getHealth() const {
        return m_health;
    }

    void Character::setHealth(int health) {
        if (m_health != health) {
            int oldHealth = m_health;
            m_health = health;
            notifyPropertyChanged("health", oldHealth, m_health);
        }
    }

    int Character::getMaxHealth() const {
        return m_maxHealth;
    }

    void Character::setMaxHealth(int maxHealth) {
        if (m_maxHealth != maxHealth) {
            int oldMax = m_maxHealth;
            m_maxHealth = maxHealth;
            notifyPropertyChanged("maxHealth", oldMax, m_maxHealth);
        }
    }

    bool Character::isPlayer() const {
        return m_isPlayer;
    }

    void Character::setPlayer(bool isPlayer) {
        if (m_isPlayer != isPlayer) {
            bool oldValue = m_isPlayer;
            m_isPlayer = isPlayer;
            notifyPropertyChanged("isPlayer", oldValue, m_isPlayer);
        }
    }

    const ImVec4& Character::getDialogColor() const {
        return m_dialogColor;
    }

    void Character::setDialogColor(const ImVec4& color) {
        if (m_dialogColor.x != color.x ||
            m_dialogColor.y != color.y ||
            m_dialogColor.z != color.z ||
            m_dialogColor.w != color.w) {
            ImVec4 oldColor = m_dialogColor;
            m_dialogColor = color;
            notifyPropertyChanged("dialogColor", oldColor, m_dialogColor);
        }
    }
}
