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
 * @file Scene.cpp
 * @brief Implementation of the Scene entity class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "Scene.h"

namespace ADS::Entities {
    Scene::Scene(const std::string& id, const std::string& name)
        : BaseEntity(id, name),
          m_isStartScene(false),
          m_backgroundColor(0.2f, 0.2f, 0.2f, 1.0f),
          m_width(800),
          m_height(600) {
    }

    std::string Scene::getTypeName() const {
        return "Scene";
    }

    std::vector<Inspector::PropertyDescriptor> Scene::getPropertyDescriptors() const {
        using namespace Inspector;

        return {
            // General category
            PropertyDescriptor("name", "Name", PropertyType::String)
                .setCategory("General")
                .setDescription("The scene's display name")
                .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("description", "Description", PropertyType::String)
                .setCategory("General")
                .setDescription("Detailed description of the scene"),

            PropertyDescriptor("isStartScene", "Start Scene", PropertyType::Bool)
                .setCategory("General")
                .setDescription("Set as the game's starting scene"),

            // Appearance category
            PropertyDescriptor("backgroundColor", "Background Color", PropertyType::Color)
                .setCategory("Appearance")
                .setDescription("Scene background color"),

            // Dimensions category
            PropertyDescriptor("width", "Width", PropertyType::Int)
                .setCategory("Dimensions")
                .setDescription("Scene width in pixels")
                .setConstraints(PropertyConstraints::numeric(1, 4096, 1)),

            PropertyDescriptor("height", "Height", PropertyType::Int)
                .setCategory("Dimensions")
                .setDescription("Scene height in pixels")
                .setConstraints(PropertyConstraints::numeric(1, 4096, 1)),

            // Info category (read-only)
            PropertyDescriptor("id", "ID", PropertyType::String)
                .setCategory("Info")
                .setDescription("Unique scene identifier")
                .setReadOnly()
        };
    }

    Inspector::PropertyValue Scene::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name") return m_name;
        if (propertyId == "description") return m_description;
        if (propertyId == "isStartScene") return m_isStartScene;
        if (propertyId == "backgroundColor") return m_backgroundColor;
        if (propertyId == "width") return m_width;
        if (propertyId == "height") return m_height;
        if (propertyId == "id") return m_id;

        return std::monostate{};
    }

    bool Scene::setPropertyValue(
        const std::string& propertyId,
        const Inspector::PropertyValue& value
    ) {
        using namespace Inspector;

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
        else if (propertyId == "isStartScene") {
            if (auto* b = std::get_if<bool>(&value)) {
                setStartScene(*b);
                return true;
            }
        }
        else if (propertyId == "backgroundColor") {
            if (auto* color = std::get_if<ImVec4>(&value)) {
                setBackgroundColor(*color);
                return true;
            }
        }
        else if (propertyId == "width") {
            if (auto* i = std::get_if<int>(&value)) {
                setWidth(*i);
                return true;
            }
        }
        else if (propertyId == "height") {
            if (auto* i = std::get_if<int>(&value)) {
                setHeight(*i);
                return true;
            }
        }

        return false;
    }

    const std::string& Scene::getDescription() const {
        return m_description;
    }

    void Scene::setDescription(const std::string& desc) {
        if (m_description != desc) {
            std::string oldDesc = m_description;
            m_description = desc;
            notifyPropertyChanged("description", oldDesc, m_description);
        }
    }

    bool Scene::isStartScene() const {
        return m_isStartScene;
    }

    void Scene::setStartScene(bool isStart) {
        if (m_isStartScene != isStart) {
            bool oldValue = m_isStartScene;
            m_isStartScene = isStart;
            notifyPropertyChanged("isStartScene", oldValue, m_isStartScene);
        }
    }

    const ImVec4& Scene::getBackgroundColor() const {
        return m_backgroundColor;
    }

    void Scene::setBackgroundColor(const ImVec4& color) {
        if (m_backgroundColor.x != color.x ||
            m_backgroundColor.y != color.y ||
            m_backgroundColor.z != color.z ||
            m_backgroundColor.w != color.w) {
            ImVec4 oldColor = m_backgroundColor;
            m_backgroundColor = color;
            notifyPropertyChanged("backgroundColor", oldColor, m_backgroundColor);
        }
    }

    int Scene::getWidth() const {
        return m_width;
    }

    void Scene::setWidth(int width) {
        if (m_width != width) {
            int oldWidth = m_width;
            m_width = width;
            notifyPropertyChanged("width", oldWidth, m_width);
        }
    }

    int Scene::getHeight() const {
        return m_height;
    }

    void Scene::setHeight(int height) {
        if (m_height != height) {
            int oldHeight = m_height;
            m_height = height;
            notifyPropertyChanged("height", oldHeight, m_height);
        }
    }
}
