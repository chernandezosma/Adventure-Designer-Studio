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
 * @file Project.cpp
 * @brief Implementation of the Project data model
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Feb 2026
 */

#include "Project.h"

#include <algorithm>

namespace ADS::Core {

    /**
     * @brief Construct a new Project with the given name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Initializes the project with an empty entity collection and stores
     * the provided human-readable name.
     *
     * @param name Human-readable display name for the project
     */
    Project::Project(const std::string& name)
        : m_name(name) {
    }

    // --- Project metadata ---

    /**
     * @brief Get the project name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Returns the human-readable display name of the project.
     *
     * @return const std::string& Reference to the project name string
     */
    const std::string& Project::getName() const {
        return m_name;
    }

    /**
     * @brief Set the project name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Replaces the current display name of the project.
     *
     * @param name New human-readable display name
     */
    void Project::setName(const std::string& name) {
        m_name = name;
    }

    // --- Scene CRUD ---

    /**
     * @brief Create and add a new Scene to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Constructs a Scene with the given id and name, takes ownership via
     * unique_ptr, and appends it to the scene collection.
     *
     * @param id   Unique identifier for the scene
     * @param name Display name for the scene
     * @return Entities::Scene* Non-owning pointer to the new scene,
     *         or nullptr if a scene with the same id already exists
     */
    Entities::Scene* Project::addScene(const std::string& id, const std::string& name) {
        if (findScene(id) != nullptr) {
            return nullptr;
        }
        m_scenes.push_back(std::make_unique<Entities::Scene>(id, name));
        return m_scenes.back().get();
    }

    /**
     * @brief Remove the scene with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Erases the scene whose id matches the argument from the collection.
     * Does nothing if no matching scene is found.
     *
     * @param id Unique identifier of the scene to remove
     */
    void Project::removeScene(const std::string& id) {
        m_scenes.erase(
            std::remove_if(m_scenes.begin(), m_scenes.end(),
                [&id](const std::unique_ptr<Entities::Scene>& s) {
                    return s->getId() == id;
                }),
            m_scenes.end()
        );
    }

    /**
     * @brief Find a scene by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Performs a linear search through the scene collection for an entry
     * whose id matches the argument.
     *
     * @param id Unique identifier to search for
     * @return Entities::Scene* Non-owning pointer to the scene, or nullptr if not found
     */
    Entities::Scene* Project::findScene(const std::string& id) const {
        for (const auto& scene : m_scenes) {
            if (scene->getId() == id) {
                return scene.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full scene collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Provides read-only access to the owned scene vector for iteration.
     *
     * @return const std::vector<std::unique_ptr<Entities::Scene>>& Owned scene vector
     */
    const std::vector<std::unique_ptr<Entities::Scene>>& Project::getScenes() const {
        return m_scenes;
    }

    // --- Character CRUD ---

    /**
     * @brief Create and add a new Character to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Constructs a Character with the given id and name, takes ownership via
     * unique_ptr, and appends it to the character collection.
     *
     * @param id   Unique identifier for the character
     * @param name Display name for the character
     * @return Entities::Character* Non-owning pointer to the new character,
     *         or nullptr if a character with the same id already exists
     */
    Entities::Character* Project::addCharacter(const std::string& id, const std::string& name) {
        if (findCharacter(id) != nullptr) {
            return nullptr;
        }
        m_characters.push_back(std::make_unique<Entities::Character>(id, name));
        return m_characters.back().get();
    }

    /**
     * @brief Remove the character with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Erases the character whose id matches the argument from the collection.
     * Does nothing if no matching character is found.
     *
     * @param id Unique identifier of the character to remove
     */
    void Project::removeCharacter(const std::string& id) {
        m_characters.erase(
            std::remove_if(m_characters.begin(), m_characters.end(),
                [&id](const std::unique_ptr<Entities::Character>& c) {
                    return c->getId() == id;
                }),
            m_characters.end()
        );
    }

    /**
     * @brief Find a character by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Performs a linear search through the character collection for an entry
     * whose id matches the argument.
     *
     * @param id Unique identifier to search for
     * @return Entities::Character* Non-owning pointer to the character, or nullptr if not found
     */
    Entities::Character* Project::findCharacter(const std::string& id) const {
        for (const auto& character : m_characters) {
            if (character->getId() == id) {
                return character.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full character collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Provides read-only access to the owned character vector for iteration.
     *
     * @return const std::vector<std::unique_ptr<Entities::Character>>& Owned character vector
     */
    const std::vector<std::unique_ptr<Entities::Character>>& Project::getCharacters() const {
        return m_characters;
    }

    // --- Item CRUD ---

    /**
     * @brief Create and add a new Item to the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Constructs an Item with the given id and name, takes ownership via
     * unique_ptr, and appends it to the item collection.
     *
     * @param id   Unique identifier for the item
     * @param name Display name for the item
     * @return Entities::Item* Non-owning pointer to the new item,
     *         or nullptr if an item with the same id already exists
     */
    Entities::Item* Project::addItem(const std::string& id, const std::string& name) {
        if (findItem(id) != nullptr) {
            return nullptr;
        }
        m_items.push_back(std::make_unique<Entities::Item>(id, name));
        return m_items.back().get();
    }

    /**
     * @brief Remove the item with the given id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Erases the item whose id matches the argument from the collection.
     * Does nothing if no matching item is found.
     *
     * @param id Unique identifier of the item to remove
     */
    void Project::removeItem(const std::string& id) {
        m_items.erase(
            std::remove_if(m_items.begin(), m_items.end(),
                [&id](const std::unique_ptr<Entities::Item>& i) {
                    return i->getId() == id;
                }),
            m_items.end()
        );
    }

    /**
     * @brief Find an item by id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Performs a linear search through the item collection for an entry
     * whose id matches the argument.
     *
     * @param id Unique identifier to search for
     * @return Entities::Item* Non-owning pointer to the item, or nullptr if not found
     */
    Entities::Item* Project::findItem(const std::string& id) const {
        for (const auto& item : m_items) {
            if (item->getId() == id) {
                return item.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the full item collection (read-only)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Provides read-only access to the owned item vector for iteration.
     *
     * @return const std::vector<std::unique_ptr<Entities::Item>>& Owned item vector
     */
    const std::vector<std::unique_ptr<Entities::Item>>& Project::getItems() const {
        return m_items;
    }

} // namespace ADS::Core
