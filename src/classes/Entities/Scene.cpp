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
 * @file Scene.cpp
 * @brief Implementation of the Scene entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "Scene.h"
#include "imgui.h"

namespace ADS::Entities {
    Scene::Scene(Data::SceneData* data)
        : BaseEntity(data), m_data(data) {
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

            PropertyDescriptor("backgroundImagePath", "Background Image", PropertyType::String)
                .setCategory("Appearance")
                .setDescription("Path to the background image asset"),

            PropertyDescriptor("musicPath", "Music", PropertyType::String)
                .setCategory("Appearance")
                .setDescription("Path to the background music asset"),

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
        if (propertyId == "name")        return m_data->name;
        if (propertyId == "description") return m_data->description;
        if (propertyId == "isStartScene") return m_data->isStartScene;
        if (propertyId == "backgroundColor") {
            const auto& c = m_data->backgroundColor;
            return ImVec4(c.r, c.g, c.b, c.a);
        }
        if (propertyId == "backgroundImagePath") return m_data->backgroundImagePath;
        if (propertyId == "musicPath")           return m_data->musicPath;
        if (propertyId == "width")               return m_data->width;
        if (propertyId == "height")              return m_data->height;
        if (propertyId == "id")                  return m_data->id;

        return std::monostate{};
    }

    bool Scene::setPropertyValue(
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
        else if (propertyId == "isStartScene") {
            if (auto* b = std::get_if<bool>(&value)) {
                setStartScene(*b);
                return true;
            }
        }
        else if (propertyId == "backgroundColor") {
            if (auto* color = std::get_if<ImVec4>(&value)) {
                setBackgroundColor({color->x, color->y, color->z, color->w});
                return true;
            }
        }
        else if (propertyId == "backgroundImagePath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setBackgroundImagePath(*str);
                return true;
            }
        }
        else if (propertyId == "musicPath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setMusicPath(*str);
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
        return m_data->description;
    }

    void Scene::setDescription(const std::string& desc) {
        if (m_data->description != desc) {
            std::string oldDesc = m_data->description;
            m_data->description = desc;
            notifyPropertyChanged("description", oldDesc, m_data->description);
        }
    }

    bool Scene::isStartScene() const {
        return m_data->isStartScene;
    }

    void Scene::setStartScene(bool isStart) {
        if (m_data->isStartScene != isStart) {
            bool oldValue = m_data->isStartScene;
            m_data->isStartScene = isStart;
            notifyPropertyChanged("isStartScene", oldValue, m_data->isStartScene);
        }
    }

    const ADS::Types::Color& Scene::getBackgroundColor() const {
        return m_data->backgroundColor;
    }

    void Scene::setBackgroundColor(const ADS::Types::Color& color) {
        if (m_data->backgroundColor != color) {
            ImVec4 oldVec(m_data->backgroundColor.r, m_data->backgroundColor.g,
                          m_data->backgroundColor.b, m_data->backgroundColor.a);
            m_data->backgroundColor = color;
            ImVec4 newVec(color.r, color.g, color.b, color.a);
            notifyPropertyChanged("backgroundColor", oldVec, newVec);
        }
    }

    int Scene::getWidth() const {
        return m_data->width;
    }

    void Scene::setWidth(int width) {
        if (m_data->width != width) {
            int oldWidth = m_data->width;
            m_data->width = width;
            notifyPropertyChanged("width", oldWidth, m_data->width);
        }
    }

    int Scene::getHeight() const {
        return m_data->height;
    }

    void Scene::setHeight(int height) {
        if (m_data->height != height) {
            int oldHeight = m_data->height;
            m_data->height = height;
            notifyPropertyChanged("height", oldHeight, m_data->height);
        }
    }

    const std::string& Scene::getBackgroundImagePath() const {
        return m_data->backgroundImagePath;
    }

    void Scene::setBackgroundImagePath(const std::string& path) {
        if (m_data->backgroundImagePath != path) {
            std::string oldPath = m_data->backgroundImagePath;
            m_data->backgroundImagePath = path;
            notifyPropertyChanged("backgroundImagePath", oldPath, m_data->backgroundImagePath);
        }
    }

    const std::string& Scene::getMusicPath() const {
        return m_data->musicPath;
    }

    void Scene::setMusicPath(const std::string& path) {
        if (m_data->musicPath != path) {
            std::string oldPath = m_data->musicPath;
            m_data->musicPath = path;
            notifyPropertyChanged("musicPath", oldPath, m_data->musicPath);
        }
    }
}
