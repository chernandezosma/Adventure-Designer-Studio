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
 * @file Project.cpp
 * @brief Implementation of the Project data model
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "Project.h"

#include <algorithm>

namespace ADS::Core {

    Project::Project(const std::string& name)
        : m_name(name) {
    }

    // --- Project metadata ---

    const std::string& Project::getName() const {
        return m_name;
    }

    void Project::setName(const std::string& name) {
        m_name = name;
    }

    // --- File path ---

    bool Project::isSaved() const {
        return m_filePath.has_value();
    }

    const std::filesystem::path& Project::getFilePath() const {
        return *m_filePath;
    }

    void Project::setFilePath(const std::filesystem::path& path) {
        m_filePath = path;
    }

    void Project::clearFilePath() {
        m_filePath.reset();
    }

    // --- Scene CRUD ---

    Entities::Scene* Project::addScene(const std::string& id, const std::string& name) {
        if (findScene(id) != nullptr) {
            return nullptr;
        }
        auto data  = std::make_unique<Data::SceneData>();
        data->id   = id;
        data->name = name;
        Data::SceneData* rawData = data.get();
        m_sceneData.push_back(std::move(data));

        auto entity     = std::make_unique<Entities::Scene>(rawData);
        Entities::Scene* rawEntity = entity.get();
        m_scenes.push_back(std::move(entity));
        return rawEntity;
    }

    void Project::removeScene(const std::string& id) {
        m_scenes.erase(
            std::remove_if(m_scenes.begin(), m_scenes.end(),
                [&id](const std::unique_ptr<Entities::Scene>& s) {
                    return s->getId() == id;
                }),
            m_scenes.end()
        );
        m_sceneData.erase(
            std::remove_if(m_sceneData.begin(), m_sceneData.end(),
                [&id](const std::unique_ptr<Data::SceneData>& d) {
                    return d->id == id;
                }),
            m_sceneData.end()
        );
    }

    Entities::Scene* Project::findScene(const std::string& id) const {
        for (const auto& scene : m_scenes) {
            if (scene->getId() == id) {
                return scene.get();
            }
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entities::Scene>>& Project::getScenes() const {
        return m_scenes;
    }

    const std::vector<std::unique_ptr<Data::SceneData>>& Project::getSceneData() const {
        return m_sceneData;
    }

    // --- Character CRUD ---

    Entities::Character* Project::addCharacter(const std::string& id, const std::string& name) {
        if (findCharacter(id) != nullptr) {
            return nullptr;
        }
        auto data  = std::make_unique<Data::CharacterData>();
        data->id   = id;
        data->name = name;
        Data::CharacterData* rawData = data.get();
        m_characterData.push_back(std::move(data));

        auto entity          = std::make_unique<Entities::Character>(rawData);
        Entities::Character* rawEntity = entity.get();
        m_characters.push_back(std::move(entity));
        return rawEntity;
    }

    void Project::removeCharacter(const std::string& id) {
        m_characters.erase(
            std::remove_if(m_characters.begin(), m_characters.end(),
                [&id](const std::unique_ptr<Entities::Character>& c) {
                    return c->getId() == id;
                }),
            m_characters.end()
        );
        m_characterData.erase(
            std::remove_if(m_characterData.begin(), m_characterData.end(),
                [&id](const std::unique_ptr<Data::CharacterData>& d) {
                    return d->id == id;
                }),
            m_characterData.end()
        );
    }

    Entities::Character* Project::findCharacter(const std::string& id) const {
        for (const auto& character : m_characters) {
            if (character->getId() == id) {
                return character.get();
            }
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entities::Character>>& Project::getCharacters() const {
        return m_characters;
    }

    const std::vector<std::unique_ptr<Data::CharacterData>>& Project::getCharacterData() const {
        return m_characterData;
    }

    // --- Item CRUD ---

    Entities::Item* Project::addItem(const std::string& id, const std::string& name) {
        if (findItem(id) != nullptr) {
            return nullptr;
        }
        auto data  = std::make_unique<Data::ItemData>();
        data->id   = id;
        data->name = name;
        Data::ItemData* rawData = data.get();
        m_itemData.push_back(std::move(data));

        auto entity      = std::make_unique<Entities::Item>(rawData);
        Entities::Item* rawEntity = entity.get();
        m_items.push_back(std::move(entity));
        return rawEntity;
    }

    void Project::removeItem(const std::string& id) {
        m_items.erase(
            std::remove_if(m_items.begin(), m_items.end(),
                [&id](const std::unique_ptr<Entities::Item>& i) {
                    return i->getId() == id;
                }),
            m_items.end()
        );
        m_itemData.erase(
            std::remove_if(m_itemData.begin(), m_itemData.end(),
                [&id](const std::unique_ptr<Data::ItemData>& d) {
                    return d->id == id;
                }),
            m_itemData.end()
        );
    }

    Entities::Item* Project::findItem(const std::string& id) const {
        for (const auto& item : m_items) {
            if (item->getId() == id) {
                return item.get();
            }
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entities::Item>>& Project::getItems() const {
        return m_items;
    }

    const std::vector<std::unique_ptr<Data::ItemData>>& Project::getItemData() const {
        return m_itemData;
    }

} // namespace ADS::Core
