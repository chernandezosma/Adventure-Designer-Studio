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
    Scene::Scene(Data::SceneData *data) :
        BaseEntity(data), m_data(data)
    {
    }

    std::string Scene::getTypeName() const
    {
        return "Scene";
    }

    std::vector<Inspector::PropertyDescriptor> Scene::getPropertyDescriptors() const
    {
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
                .setReadOnly(),

                PropertyDescriptor("fakeProperty", "Fake Property", PropertyType::Bool)
                .setCategory("Info")
                .setDescription("Fake description to test"),
        };
    }

    Inspector::PropertyValue Scene::getPropertyValue(const std::string &propertyId) const
    {
        if (propertyId == "name")
            return m_data->getName();
        if (propertyId == "description")
            return m_data->getDescription();
        if (propertyId == "isStartScene")
            return m_data->isStartScene();
        if (propertyId == "backgroundColor") {
            const auto &c = m_data->getBackgroundColor();
            return ImVec4(c.r, c.g, c.b, c.a);
        }
        if (propertyId == "backgroundImagePath")
            return m_data->getBackgroundImagePath();
        if (propertyId == "musicPath")
            return m_data->getMusicPath();
        if (propertyId == "width")
            return m_data->getWidth();
        if (propertyId == "height")
            return m_data->getHeight();
        if (propertyId == "id")
            return m_data->getId();

        if (propertyId == "fakeProperty")
            return m_data->getFakeProperty();

        return std::monostate{};
    }

    bool Scene::setPropertyValue(const std::string &propertyId, const Inspector::PropertyValue &value)
    {
        if (propertyId == "name") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setName(*str);
                return true;
            }
        } else if (propertyId == "description") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setDescription(*str);
                return true;
            }
        } else if (propertyId == "isStartScene") {
            if (auto *b = std::get_if<bool>(&value)) {
                setStartScene(*b);
                return true;
            }
        } else if (propertyId == "backgroundColor") {
            if (auto *color = std::get_if<ImVec4>(&value)) {
                setBackgroundColor({color->x, color->y, color->z, color->w});
                return true;
            }
        } else if (propertyId == "backgroundImagePath") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setBackgroundImagePath(*str);
                return true;
            }
        } else if (propertyId == "musicPath") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setMusicPath(*str);
                return true;
            }
        } else if (propertyId == "width") {
            if (auto *i = std::get_if<int>(&value)) {
                setWidth(*i);
                return true;
            }
        } else if (propertyId == "height") {
            if (auto *i = std::get_if<int>(&value)) {
                setHeight(*i);
                return true;
            }
        } else if (propertyId == "fakeProperty") {
            if (auto *i = std::get_if<bool>(&value)) {
                setFakeProperty(*i);
                return true;
            }
        }


        return false;
    }

    const bool Scene::getFakeProperty()
    {
        return m_data->getFakeProperty();
    }

    void Scene::setFakeProperty(const bool &value)
    {
        setAndNotify("fakeProperty",
                     [this] {
                         return m_data->getFakeProperty();
                     },
                     [this](const bool &v) {
                         m_data->setFakeProperty(v);
                     },
                     value);
    }

    const std::string &Scene::getDescription() const
    {
        return m_data->getDescription();
    }

    void Scene::setDescription(const std::string &desc)
    {
        setAndNotify("description",
                     [this] {
                         return m_data->getDescription();
                     },
                     [this](const std::string &v) {
                         m_data->setDescription(v);
                     },
                     desc);
    }

    bool Scene::isStartScene() const
    {
        return m_data->isStartScene();
    }

    void Scene::setStartScene(bool isStart)
    {
        setAndNotify("isStartScene",
                     [this] {
                         return m_data->isStartScene();
                     },
                     [this](bool v) {
                         m_data->setStartScene(v);
                     },
                     isStart);
    }

    const ADS::Types::Color &Scene::getBackgroundColor() const
    {
        return m_data->getBackgroundColor();
    }

    void Scene::setBackgroundColor(const ADS::Types::Color &color)
    {
        if (m_data->getBackgroundColor() != color) {
            const auto &old = m_data->getBackgroundColor();
            ImVec4 oldVec(old.r, old.g, old.b, old.a);
            m_data->setBackgroundColor(color);
            ImVec4 newVec(color.r, color.g, color.b, color.a);
            notifyPropertyChanged("backgroundColor", oldVec, newVec);
        }
    }

    int Scene::getWidth() const
    {
        return m_data->getWidth();
    }

    void Scene::setWidth(int width)
    {
        setAndNotify("width",
                     [this] {
                         return m_data->getWidth();
                     },
                     [this](int v) {
                         m_data->setWidth(v);
                     },
                     width);
    }

    int Scene::getHeight() const
    {
        return m_data->getHeight();
    }

    void Scene::setHeight(int height)
    {
        setAndNotify("height",
                     [this] {
                         return m_data->getHeight();
                     },
                     [this](int v) {
                         m_data->setHeight(v);
                     },
                     height);
    }

    const std::string &Scene::getBackgroundImagePath() const
    {
        return m_data->getBackgroundImagePath();
    }

    void Scene::setBackgroundImagePath(const std::string &path)
    {
        setAndNotify("backgroundImagePath",
                     [this] {
                         return m_data->getBackgroundImagePath();
                     },
                     [this](const std::string &v) {
                         m_data->setBackgroundImagePath(v);
                     },
                     path);
    }

    const std::string &Scene::getMusicPath() const
    {
        return m_data->getMusicPath();
    }

    void Scene::setMusicPath(const std::string &path)
    {
        setAndNotify("musicPath",
                     [this] {
                         return m_data->getMusicPath();
                     },
                     [this](const std::string &v) {
                         m_data->setMusicPath(v);
                     },
                     path);
    }
}
