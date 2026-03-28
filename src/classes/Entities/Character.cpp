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
 * @file Character.cpp
 * @brief Implementation of the Character entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "Character.h"
#include "imgui.h"

namespace ADS::Entities {
    Character::Character(Data::CharacterData* data)
        : BaseEntity(data), m_data(data) {
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

            PropertyDescriptor("startingSceneId", "Starting Scene", PropertyType::String)
                .setCategory("General")
                .setDescription("ID of the scene where this character starts"),

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

            PropertyDescriptor("portraitPath", "Portrait", PropertyType::String)
                .setCategory("Appearance")
                .setDescription("Path to the character portrait image"),

            // Info category (read-only)
            PropertyDescriptor("id", "ID", PropertyType::String)
                .setCategory("Info")
                .setDescription("Unique character identifier")
                .setReadOnly()
        };
    }

    Inspector::PropertyValue Character::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name")        return m_data->name;
        if (propertyId == "description") return m_data->description;
        if (propertyId == "isPlayer")    return m_data->isPlayer;
        if (propertyId == "health")      return m_data->health;
        if (propertyId == "maxHealth")   return m_data->maxHealth;
        if (propertyId == "dialogColor") {
            const auto& c = m_data->dialogColor;
            return ImVec4(c.r, c.g, c.b, c.a);
        }
        if (propertyId == "portraitPath")    return m_data->portraitPath;
        if (propertyId == "startingSceneId") return m_data->startingSceneId;
        if (propertyId == "id")              return m_data->id;

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
                setDialogColor({color->x, color->y, color->z, color->w});
                return true;
            }
        }
        else if (propertyId == "portraitPath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setPortraitPath(*str);
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

    const std::string& Character::getDescription() const {
        return m_data->description;
    }

    void Character::setDescription(const std::string& desc) {
        if (m_data->description != desc) {
            std::string oldDesc = m_data->description;
            m_data->description = desc;
            notifyPropertyChanged("description", oldDesc, m_data->description);
        }
    }

    int Character::getHealth() const {
        return m_data->health;
    }

    void Character::setHealth(int health) {
        if (m_data->health != health) {
            int oldHealth = m_data->health;
            m_data->health = health;
            notifyPropertyChanged("health", oldHealth, m_data->health);
        }
    }

    int Character::getMaxHealth() const {
        return m_data->maxHealth;
    }

    void Character::setMaxHealth(int maxHealth) {
        if (m_data->maxHealth != maxHealth) {
            int oldMax = m_data->maxHealth;
            m_data->maxHealth = maxHealth;
            notifyPropertyChanged("maxHealth", oldMax, m_data->maxHealth);
        }
    }

    bool Character::isPlayer() const {
        return m_data->isPlayer;
    }

    void Character::setPlayer(bool isPlayer) {
        if (m_data->isPlayer != isPlayer) {
            bool oldValue = m_data->isPlayer;
            m_data->isPlayer = isPlayer;
            notifyPropertyChanged("isPlayer", oldValue, m_data->isPlayer);
        }
    }

    const ADS::Types::Color& Character::getDialogColor() const {
        return m_data->dialogColor;
    }

    void Character::setDialogColor(const ADS::Types::Color& color) {
        if (m_data->dialogColor != color) {
            ImVec4 oldVec(m_data->dialogColor.r, m_data->dialogColor.g,
                          m_data->dialogColor.b, m_data->dialogColor.a);
            m_data->dialogColor = color;
            ImVec4 newVec(color.r, color.g, color.b, color.a);
            notifyPropertyChanged("dialogColor", oldVec, newVec);
        }
    }

    const std::string& Character::getPortraitPath() const {
        return m_data->portraitPath;
    }

    void Character::setPortraitPath(const std::string& path) {
        if (m_data->portraitPath != path) {
            std::string oldPath = m_data->portraitPath;
            m_data->portraitPath = path;
            notifyPropertyChanged("portraitPath", oldPath, m_data->portraitPath);
        }
    }

    const std::string& Character::getStartingSceneId() const {
        return m_data->startingSceneId;
    }

    void Character::setStartingSceneId(const std::string& sceneId) {
        if (m_data->startingSceneId != sceneId) {
            std::string oldId = m_data->startingSceneId;
            m_data->startingSceneId = sceneId;
            notifyPropertyChanged("startingSceneId", oldId, m_data->startingSceneId);
        }
    }
}
