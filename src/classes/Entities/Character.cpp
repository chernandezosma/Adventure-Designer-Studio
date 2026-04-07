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
        if (propertyId == "name")        return m_data->getName();
        if (propertyId == "description") return m_data->getDescription();
        if (propertyId == "isPlayer")    return m_data->isPlayer();
        if (propertyId == "health")      return m_data->getHealth();
        if (propertyId == "maxHealth")   return m_data->getMaxHealth();
        if (propertyId == "dialogColor") {
            const auto& c = m_data->getDialogColor();
            return ImVec4(c.r, c.g, c.b, c.a);
        }
        if (propertyId == "portraitPath")    return m_data->getPortraitPath();
        if (propertyId == "startingSceneId") return m_data->getStartingSceneId();
        if (propertyId == "id")              return m_data->getId();

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
        return m_data->getDescription();
    }

    void Character::setDescription(const std::string& desc) {
        setAndNotify("description",
            [this]{ return m_data->getDescription(); },
            [this](const std::string& v){ m_data->setDescription(v); },
            desc);
    }

    int Character::getHealth() const {
        return m_data->getHealth();
    }

    void Character::setHealth(int health) {
        setAndNotify("health",
            [this]{ return m_data->getHealth(); },
            [this](int v){ m_data->setHealth(v); },
            health);
    }

    int Character::getMaxHealth() const {
        return m_data->getMaxHealth();
    }

    void Character::setMaxHealth(int maxHealth) {
        setAndNotify("maxHealth",
            [this]{ return m_data->getMaxHealth(); },
            [this](int v){ m_data->setMaxHealth(v); },
            maxHealth);
    }

    bool Character::isPlayer() const {
        return m_data->isPlayer();
    }

    void Character::setPlayer(bool isPlayer) {
        setAndNotify("isPlayer",
            [this]{ return m_data->isPlayer(); },
            [this](bool v){ m_data->setPlayer(v); },
            isPlayer);
    }

    const ADS::Types::Color& Character::getDialogColor() const {
        return m_data->getDialogColor();
    }

    void Character::setDialogColor(const ADS::Types::Color& color) {
        if (m_data->getDialogColor() != color) {
            const auto& old = m_data->getDialogColor();
            ImVec4 oldVec(old.r, old.g, old.b, old.a);
            m_data->setDialogColor(color);
            ImVec4 newVec(color.r, color.g, color.b, color.a);
            notifyPropertyChanged("dialogColor", oldVec, newVec);
        }
    }

    const std::string& Character::getPortraitPath() const {
        return m_data->getPortraitPath();
    }

    void Character::setPortraitPath(const std::string& path) {
        setAndNotify("portraitPath",
            [this]{ return m_data->getPortraitPath(); },
            [this](const std::string& v){ m_data->setPortraitPath(v); },
            path);
    }

    const std::string& Character::getStartingSceneId() const {
        return m_data->getStartingSceneId();
    }

    void Character::setStartingSceneId(const std::string& sceneId) {
        setAndNotify("startingSceneId",
            [this]{ return m_data->getStartingSceneId(); },
            [this](const std::string& v){ m_data->setStartingSceneId(v); },
            sceneId);
    }
}