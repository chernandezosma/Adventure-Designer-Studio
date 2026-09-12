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
#include "spdlog/spdlog.h"
#include "DataSerialization.h"

namespace ADS::Core {

    Project::Project(const std::string& name) {
        m_gameData.setTitle(name);
    }

    // --- Project metadata ---

    /**
     * @brief Get the project name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The project has no separate name of its own: this delegates to
     * `getGameData().getTitle()`, which is the single source of truth
     * (docs/core/schemas/game.md `game.title`). Shown as the project-tree
     * root label and in the status bar.
     *
     * @return const std::string& The game title; empty only for a
     *         hand-edited file with no `game.title`
     */
    const std::string& Project::getName() const {
        return m_gameData.getTitle();
    }

    /**
     * @brief Set the project name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Delegates to `getGameData().setTitle(name)` — see getName().
     *
     * @param name New human-readable display name / game title
     */
    void Project::setName(const std::string& name) {
        m_gameData.setTitle(name);
    }

    // --- File path ---

    /**
     * @brief Check whether this project has been saved to disk
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return bool True if a file path is set, false for an unsaved project
     * @see setFilePath()
     */
    bool Project::isSaved() const {
        return m_filePath.has_value();
    }

    /**
     * @brief Get the path to the project file on disk
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::filesystem::path& Reference to the stored file path
     * @see isSaved(), setFilePath()
     */
    const std::filesystem::path& Project::getFilePath() const {
        return *m_filePath;
    }

    /**
     * @brief Associate a filesystem path with this project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param path Absolute or relative path to the project file
     * @see isSaved(), getFilePath(), clearFilePath()
     */
    void Project::setFilePath(const std::filesystem::path& path) {
        m_filePath = path;
    }

    /**
     * @brief Remove the associated file path from this project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @see isSaved(), setFilePath()
     */
    void Project::clearFilePath() {
        m_filePath.reset();
    }

    // --- Scene CRUD ---

    /**
     * @brief Create and add a new Scene (DataObject + entity adapter) to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id   Unique identifier for the scene
     * @param name Display name for the scene
     * @return Entities::Scene* Non-owning pointer to the entity adapter,
     *         or nullptr if a scene with the same id already exists
     */
    Entities::Scene* Project::addScene(ADS::Types::SceneId id, const std::string& name) {
        if (findScene(id) != nullptr) {
            spdlog::warn("Project: rejected duplicate Scene id '{}'", static_cast<int>(id.value));
            return nullptr;
        }
        auto data = std::make_unique<Data::SceneData>();
        data->setId(id);
        data->setName(name);
        Data::SceneData* rawData = data.get();
        m_sceneData.push_back(std::move(data));

        auto entity     = std::make_unique<Entities::Scene>(rawData);
        Entities::Scene* rawEntity = entity.get();
        rawEntity->setProject(this);
        m_scenes.push_back(std::move(entity));
        spdlog::info("Project: added Scene '{}' ({})", static_cast<int>(id.value), name);
        return rawEntity;
    }

    /**
     * @brief Remove the scene with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Erases from both the entity adapter collection and the DataObject
     * collection. Does nothing if no matching scene is found.
     *
     * @param id Unique identifier of the scene to remove
     */
    void Project::removeScene(ADS::Types::SceneId id) {
        spdlog::info("Project: removing Scene '{}'", static_cast<int>(id.value));
        m_scenes.erase(
            std::remove_if(m_scenes.begin(), m_scenes.end(),
                [id](const std::unique_ptr<Entities::Scene>& s) {
                    return s->getSceneId() == id;
                }),
            m_scenes.end()
        );
        m_sceneData.erase(
            std::remove_if(m_sceneData.begin(), m_sceneData.end(),
                [id](const std::unique_ptr<Data::SceneData>& d) {
                    return d->getId() == id;
                }),
            m_sceneData.end()
        );
    }

    /**
     * @brief Find a scene entity adapter by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Unique identifier to search for
     * @return Entities::Scene* Non-owning pointer, or nullptr if not found
     */
    Entities::Scene* Project::findScene(ADS::Types::SceneId id) const {
        for (const auto& scene : m_scenes) {
            if (scene->getSceneId() == id) {
                return scene.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full scene entity adapter collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Entities::Scene>>&
     */
    const std::vector<std::unique_ptr<Entities::Scene>>& Project::getScenes() const {
        return m_scenes;
    }

    /**
     * @brief Get the full scene DataObject collection (read-only, for serialisation)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Data::SceneData>>&
     */
    const std::vector<std::unique_ptr<Data::SceneData>>& Project::getSceneData() const {
        return m_sceneData;
    }

    // --- Character CRUD ---

    /**
     * @brief Create and add a new Character (DataObject + entity adapter) to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id   Unique identifier for the character
     * @param name Display name for the character
     * @return Entities::Character* Non-owning pointer to the entity adapter,
     *         or nullptr if a character with the same id already exists
     */
    Entities::Character* Project::addCharacter(ADS::Types::CharacterId id, const std::string& name) {
        if (findCharacter(id) != nullptr) {
            spdlog::warn("Project: rejected duplicate Character id '{}'", static_cast<int>(id.value));
            return nullptr;
        }
        auto data = std::make_unique<Data::CharacterData>();
        data->setId(id);
        data->setName(name);
        Data::CharacterData* rawData = data.get();
        m_characterData.push_back(std::move(data));

        auto entity          = std::make_unique<Entities::Character>(rawData);
        Entities::Character* rawEntity = entity.get();
        rawEntity->setProject(this);
        m_characters.push_back(std::move(entity));
        spdlog::info("Project: added Character '{}' ({})", static_cast<int>(id.value), name);
        return rawEntity;
    }

    /**
     * @brief Remove the character with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Unique identifier of the character to remove
     */
    void Project::removeCharacter(ADS::Types::CharacterId id) {
        spdlog::info("Project: removing Character '{}'", static_cast<int>(id.value));
        m_characters.erase(
            std::remove_if(m_characters.begin(), m_characters.end(),
                [id](const std::unique_ptr<Entities::Character>& c) {
                    return c->getCharacterId() == id;
                }),
            m_characters.end()
        );
        m_characterData.erase(
            std::remove_if(m_characterData.begin(), m_characterData.end(),
                [id](const std::unique_ptr<Data::CharacterData>& d) {
                    return d->getId() == id;
                }),
            m_characterData.end()
        );
    }

    /**
     * @brief Find a character entity adapter by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Unique identifier to search for
     * @return Entities::Character* Non-owning pointer, or nullptr if not found
     */
    Entities::Character* Project::findCharacter(ADS::Types::CharacterId id) const {
        for (const auto& character : m_characters) {
            if (character->getCharacterId() == id) {
                return character.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full character entity adapter collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Entities::Character>>&
     */
    const std::vector<std::unique_ptr<Entities::Character>>& Project::getCharacters() const {
        return m_characters;
    }

    /**
     * @brief Get the full character DataObject collection (read-only, for serialisation)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Data::CharacterData>>&
     */
    const std::vector<std::unique_ptr<Data::CharacterData>>& Project::getCharacterData() const {
        return m_characterData;
    }

    // --- Item CRUD ---

    /**
     * @brief Create and add a new Item (DataObject + entity adapter) to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id   Unique identifier for the item
     * @param name Display name for the item
     * @return Entities::Item* Non-owning pointer to the entity adapter,
     *         or nullptr if an item with the same id already exists
     */
    Entities::Item* Project::addItem(ADS::Types::ObjectId id, const std::string& name) {
        if (findItem(id) != nullptr) {
            spdlog::warn("Project: rejected duplicate Item id '{}'", static_cast<int>(id.value));
            return nullptr;
        }
        auto data = std::make_unique<Data::ItemData>();
        data->setId(id);
        data->setName(name);
        Data::ItemData* rawData = data.get();
        m_itemData.push_back(std::move(data));

        auto entity      = std::make_unique<Entities::Item>(rawData);
        Entities::Item* rawEntity = entity.get();
        rawEntity->setProject(this);
        m_items.push_back(std::move(entity));
        spdlog::info("Project: added Item '{}' ({})", static_cast<int>(id.value), name);
        return rawEntity;
    }

    /**
     * @brief Remove the item with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Unique identifier of the item to remove
     */
    void Project::removeItem(ADS::Types::ObjectId id) {
        spdlog::info("Project: removing Item '{}'", static_cast<int>(id.value));
        m_items.erase(
            std::remove_if(m_items.begin(), m_items.end(),
                [id](const std::unique_ptr<Entities::Item>& i) {
                    return i->getItemId() == id;
                }),
            m_items.end()
        );
        m_itemData.erase(
            std::remove_if(m_itemData.begin(), m_itemData.end(),
                [id](const std::unique_ptr<Data::ItemData>& d) {
                    return d->getId() == id;
                }),
            m_itemData.end()
        );
    }

    /**
     * @brief Find an item entity adapter by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Unique identifier to search for
     * @return Entities::Item* Non-owning pointer, or nullptr if not found
     */
    Entities::Item* Project::findItem(ADS::Types::ObjectId id) const {
        for (const auto& item : m_items) {
            if (item->getItemId() == id) {
                return item.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full item entity adapter collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Entities::Item>>&
     */
    const std::vector<std::unique_ptr<Entities::Item>>& Project::getItems() const {
        return m_items;
    }

    /**
     * @brief Get the full item DataObject collection (read-only, for serialisation)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<std::unique_ptr<Data::ItemData>>&
     */
    const std::vector<std::unique_ptr<Data::ItemData>>& Project::getItemData() const {
        return m_itemData;
    }

    // --- State CRUD ---

    /**
     * @brief Create and add a new State (DataObject + entity adapter) to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id   Unique identifier for the state
     * @param name Display name for the state
     * @return Entities::State* Non-owning pointer to the entity adapter,
     *         or nullptr if a state with the same id already exists
     */
    Entities::State* Project::addState(ADS::Types::StateId id, const std::string& name) {
        if (findState(id) != nullptr) {
            spdlog::warn("Project: rejected duplicate State id '{}'", static_cast<int>(id.value));
            return nullptr;
        }
        auto data = std::make_unique<Data::StateData>();
        data->setId(id);
        data->setName(name);
        Data::StateData* rawData = data.get();
        m_stateData.push_back(std::move(data));

        auto entity      = std::make_unique<Entities::State>(rawData);
        Entities::State* rawEntity = entity.get();
        rawEntity->setProject(this);
        m_states.push_back(std::move(entity));
        spdlog::info("Project: added State '{}' ({})", static_cast<int>(id.value), name);
        return rawEntity;
    }

    /**
     * @brief Remove the state with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id Unique identifier of the state to remove
     */
    void Project::removeState(ADS::Types::StateId id) {
        spdlog::info("Project: removing State '{}'", static_cast<int>(id.value));
        m_states.erase(
            std::remove_if(m_states.begin(), m_states.end(),
                [id](const std::unique_ptr<Entities::State>& s) {
                    return s->getStateId() == id;
                }),
            m_states.end()
        );
        m_stateData.erase(
            std::remove_if(m_stateData.begin(), m_stateData.end(),
                [id](const std::unique_ptr<Data::StateData>& d) {
                    return d->getId() == id;
                }),
            m_stateData.end()
        );
    }

    /**
     * @brief Find a state entity adapter by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id Unique identifier to search for
     * @return Entities::State* Non-owning pointer, or nullptr if not found
     */
    Entities::State* Project::findState(ADS::Types::StateId id) const {
        for (const auto& state : m_states) {
            if (state->getStateId() == id) {
                return state.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full state entity adapter collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::vector<std::unique_ptr<Entities::State>>&
     */
    const std::vector<std::unique_ptr<Entities::State>>& Project::getStates() const {
        return m_states;
    }

    /**
     * @brief Get the full state DataObject collection (read-only, for serialisation)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::vector<std::unique_ptr<Data::StateData>>&
     */
    const std::vector<std::unique_ptr<Data::StateData>>& Project::getStateData() const {
        return m_stateData;
    }

    /**
     * @brief Get the states available to reference as a "next" state or a chain's head
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * A state is "free" if no other state's @c next already points to
     * it and no chain's @c head already points to it — every state can
     * only be claimed by one place in one chain, so chains never branch
     * or overlap. Two exceptions keep an already-assigned field from
     * losing its own current value out from under it:
     * @p excludeSelf (a State can't name itself as its own next) and
     * @p keepEvenIfUsed (the field's own current value stays visible/
     * selectable so re-opening the dropdown doesn't hide what's already
     * chosen there).
     *
     * @param excludeSelf Id to omit outright (e.g. the State being
     *                    edited), or std::nullopt for none
     * @param keepEvenIfUsed Id to include regardless of "used" status
     *                       (e.g. the field's current value), or
     *                       std::nullopt for none
     * @return std::vector<Entities::State*> Non-owning pointers, in project order
     */
    std::vector<Entities::State*> Project::getFreeStates(
        std::optional<ADS::Types::StateId> excludeSelf,
        std::optional<ADS::Types::StateId> keepEvenIfUsed) const
    {
        std::vector<Entities::State*> result;
        for (const auto& state : m_states) {
            ADS::Types::StateId id = state->getStateId();
            if (excludeSelf.has_value() && id == *excludeSelf) continue;

            if (keepEvenIfUsed.has_value() && id == *keepEvenIfUsed) {
                result.push_back(state.get());
                continue;
            }

            bool used = false;
            for (const auto& s : m_stateData) {
                if (s->getNext().has_value() && *s->getNext() == id) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                for (const auto& c : m_chainData) {
                    if (c->getHead().has_value() && *c->getHead() == id) {
                        used = true;
                        break;
                    }
                }
            }
            if (!used) result.push_back(state.get());
        }
        return result;
    }

    // --- StateChain CRUD ---

    /**
     * @brief Create and add a new StateChain (DataObject + entity adapter) to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id   Unique identifier for the chain
     * @param name Display name for the chain
     * @return Entities::StateChain* Non-owning pointer to the entity
     *         adapter, or nullptr if a chain with the same id already exists
     */
    Entities::StateChain* Project::addChain(ADS::Types::ChainId id, const std::string& name) {
        if (findChain(id) != nullptr) {
            spdlog::warn("Project: rejected duplicate StateChain id '{}'", static_cast<int>(id.value));
            return nullptr;
        }
        auto data = std::make_unique<Data::StateChainData>();
        data->setId(id);
        data->setName(name);
        Data::StateChainData* rawData = data.get();
        m_chainData.push_back(std::move(data));

        auto entity      = std::make_unique<Entities::StateChain>(rawData);
        Entities::StateChain* rawEntity = entity.get();
        rawEntity->setProject(this);
        m_chains.push_back(std::move(entity));
        spdlog::info("Project: added StateChain '{}' ({})", static_cast<int>(id.value), name);
        return rawEntity;
    }

    /**
     * @brief Remove the chain with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id Unique identifier of the chain to remove
     */
    void Project::removeChain(ADS::Types::ChainId id) {
        spdlog::info("Project: removing StateChain '{}'", static_cast<int>(id.value));
        m_chains.erase(
            std::remove_if(m_chains.begin(), m_chains.end(),
                [id](const std::unique_ptr<Entities::StateChain>& c) {
                    return c->getChainId() == id;
                }),
            m_chains.end()
        );
        m_chainData.erase(
            std::remove_if(m_chainData.begin(), m_chainData.end(),
                [id](const std::unique_ptr<Data::StateChainData>& d) {
                    return d->getId() == id;
                }),
            m_chainData.end()
        );
    }

    /**
     * @brief Find a chain entity adapter by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param id Unique identifier to search for
     * @return Entities::StateChain* Non-owning pointer, or nullptr if not found
     */
    Entities::StateChain* Project::findChain(ADS::Types::ChainId id) const {
        for (const auto& chain : m_chains) {
            if (chain->getChainId() == id) {
                return chain.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full chain entity adapter collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::vector<std::unique_ptr<Entities::StateChain>>&
     */
    const std::vector<std::unique_ptr<Entities::StateChain>>& Project::getChains() const {
        return m_chains;
    }

    /**
     * @brief Get the full chain DataObject collection (read-only, for serialisation)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::vector<std::unique_ptr<Data::StateChainData>>&
     */
    const std::vector<std::unique_ptr<Data::StateChainData>>& Project::getChainData() const {
        return m_chainData;
    }

    // --- ID allocation ---

    namespace {

        /**
         * @brief Lowest value in [1, 255] not returned by @p getId for any
         *        element of @p collection.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @tparam Collection Range of `std::unique_ptr<Entity>`
         * @tparam IdGetter   Callable `uint8_t(const Entity&)`
         * @param collection Entities whose ids are already taken
         * @param getId      Extracts the raw id value from one entity
         * @return uint8_t The free id, or 255 when the space is exhausted
         */
        template <typename Collection, typename IdGetter>
        uint8_t firstFreeId(const Collection& collection, IdGetter getId) {
            bool used[256] = {};
            for (const auto& entity : collection) {
                used[getId(*entity)] = true;
            }
            for (int value = 1; value < 256; ++value) {
                if (!used[value]) {
                    return static_cast<uint8_t>(value);
                }
            }
            spdlog::warn("Project: id space exhausted — reusing 255");
            return 255;
        }

    } // namespace

    /**
     * @brief Smallest unused scene id in this project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Scans the current scene collection and returns the lowest value in
     * `[1, 255]` not already taken, so a freshly created scene never
     * collides with one loaded from disk. Falls back to `255` only when
     * every id is in use.
     *
     * @return ADS::Types::SceneId The id to give the next new scene
     */
    ADS::Types::SceneId Project::nextSceneId() const {
        return ADS::Types::SceneId(firstFreeId(m_scenes,
            [](const Entities::Scene& s) { return s.getSceneId().value; }));
    }

    /**
     * @brief Smallest unused character id in this project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return ADS::Types::CharacterId The id to give the next new character
     * @see nextSceneId()
     */
    ADS::Types::CharacterId Project::nextCharacterId() const {
        return ADS::Types::CharacterId(firstFreeId(m_characters,
            [](const Entities::Character& c) { return c.getCharacterId().value; }));
    }

    /**
     * @brief Smallest unused item id in this project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return ADS::Types::ObjectId The id to give the next new item
     * @see nextSceneId()
     */
    ADS::Types::ObjectId Project::nextItemId() const {
        return ADS::Types::ObjectId(firstFreeId(m_items,
            [](const Entities::Item& i) { return i.getItemId().value; }));
    }

    /**
     * @brief Smallest unused state id in this project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return ADS::Types::StateId The id to give the next new state
     * @see nextSceneId()
     */
    ADS::Types::StateId Project::nextStateId() const {
        return ADS::Types::StateId(firstFreeId(m_states,
            [](const Entities::State& s) { return s.getStateId().value; }));
    }

    /**
     * @brief Smallest unused state-chain id in this project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return ADS::Types::ChainId The id to give the next new chain
     * @see nextSceneId()
     */
    ADS::Types::ChainId Project::nextChainId() const {
        return ADS::Types::ChainId(firstFreeId(m_chains,
            [](const Entities::StateChain& c) { return c.getChainId().value; }));
    }

    // --- Duplication ---

    namespace {

        /**
         * @brief Find the DataObject with the given id in @p data.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @tparam DataVec Range of `std::unique_ptr<DataObject>` (BaseData-derived)
         * @tparam Id      The matching id type
         * @param data Collection to search
         * @param id   Id to match against `->getId()`
         * @return Pointer to the element, or nullptr when absent
         */
        template <typename DataVec, typename Id>
        auto findData(const DataVec& data, Id id) -> decltype(data.front().get()) {
            for (const auto& d : data) {
                if (d->getId() == id) {
                    return d.get();
                }
            }
            return nullptr;
        }

    } // namespace

    /**
     * @brief Deep-copy a scene into a new one.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Creates a scene with a fresh id (nextSceneId()) and @p newName, then
     * copies every other field from the scene identified by @p sourceId via
     * `Data::toJson` / `Data::applyJson` (the same round-trip the project
     * serialiser uses). The "start scene" flag is intentionally **not**
     * carried over — a project has exactly one start scene.
     *
     * @param sourceId Id of the scene to copy
     * @param newName  Display name for the copy
     * @return Entities::Scene* The new adapter, or nullptr if @p sourceId
     *         is unknown or the id space is exhausted
     */
    Entities::Scene* Project::duplicateScene(ADS::Types::SceneId sourceId,
                                             const std::string& newName) {
        const Data::SceneData* src = findData(m_sceneData, sourceId);
        if (src == nullptr) {
            spdlog::warn("Project: duplicateScene — unknown source id {}", sourceId.value);
            return nullptr;
        }
        Entities::Scene* copy = addScene(nextSceneId(), newName);
        if (copy == nullptr) {
            return nullptr;
        }
        Data::applyJson(Data::toJson(*src), *m_sceneData.back());
        m_sceneData.back()->setStartScene(false);
        return copy;
    }

    /**
     * @brief Deep-copy a character into a new one.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * As duplicateScene(), but the "is player" flag is not carried over —
     * a project has at most one player character.
     *
     * @param sourceId Id of the character to copy
     * @param newName  Display name for the copy
     * @return Entities::Character* The new adapter, or nullptr on failure
     */
    Entities::Character* Project::duplicateCharacter(ADS::Types::CharacterId sourceId,
                                                     const std::string& newName) {
        const Data::CharacterData* src = findData(m_characterData, sourceId);
        if (src == nullptr) {
            spdlog::warn("Project: duplicateCharacter — unknown source id {}", sourceId.value);
            return nullptr;
        }
        Entities::Character* copy = addCharacter(nextCharacterId(), newName);
        if (copy == nullptr) {
            return nullptr;
        }
        Data::applyJson(Data::toJson(*src), *m_characterData.back());
        m_characterData.back()->setPlayer(false);
        return copy;
    }

    /**
     * @brief Deep-copy an item into a new one.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param sourceId Id of the item to copy
     * @param newName  Display name for the copy
     * @return Entities::Item* The new adapter, or nullptr on failure
     */
    Entities::Item* Project::duplicateItem(ADS::Types::ObjectId sourceId,
                                           const std::string& newName) {
        const Data::ItemData* src = findData(m_itemData, sourceId);
        if (src == nullptr) {
            spdlog::warn("Project: duplicateItem — unknown source id {}", sourceId.value);
            return nullptr;
        }
        Entities::Item* copy = addItem(nextItemId(), newName);
        if (copy == nullptr) {
            return nullptr;
        }
        Data::applyJson(Data::toJson(*src), *m_itemData.back());
        return copy;
    }

    /**
     * @brief Deep-copy a state into a new one.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param sourceId Id of the state to copy
     * @param newName  Display name for the copy
     * @return Entities::State* The new adapter, or nullptr on failure
     */
    Entities::State* Project::duplicateState(ADS::Types::StateId sourceId,
                                             const std::string& newName) {
        const Data::StateData* src = findData(m_stateData, sourceId);
        if (src == nullptr) {
            spdlog::warn("Project: duplicateState — unknown source id {}", sourceId.value);
            return nullptr;
        }
        Entities::State* copy = addState(nextStateId(), newName);
        if (copy == nullptr) {
            return nullptr;
        }
        Data::applyJson(Data::toJson(*src), *m_stateData.back());
        return copy;
    }

    // --- LexEngine ---

    /**
     * @brief Get the project's LexEngine (vocabulary compiler subsystem)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Unlike Scene/Character/Item, the LexEngine is a single instance per
     * project rather than a collection — there is no per-entry inspector
     * adapter for individual vocabulary entries.
     *
     * @return LexEngine::LexEngine& Reference to the owned LexEngine
     */
    LexEngine::LexEngine& Project::getLexEngine() const {
        return *m_lexEngine;
    }

    // --- Game settings ---

    /**
     * @brief Get the project-wide game settings (read-only).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * A project has exactly one game entry (docs/core/schemas/game.md).
     *
     * @return const Data::GameData& The game settings
     */
    const Data::GameData& Project::getGameData() const {
        return m_gameData;
    }

    /**
     * @brief Get the project-wide game settings (mutable).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * There is no entity adapter for game settings yet — callers edit the
     * DataObject directly.
     *
     * @return Data::GameData& The game settings
     */
    Data::GameData& Project::getGameData() {
        return m_gameData;
    }

} // namespace ADS::Core
