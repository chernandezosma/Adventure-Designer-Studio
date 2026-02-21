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

#ifndef ADS_CORE_PROJECT_H
#define ADS_CORE_PROJECT_H

/**
 * @file Project.h
 * @brief Top-level application data model that owns all game entities
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Feb 2026
 *
 * The Project class is the runtime container for a game project. It owns
 * collections of Scene, Character, and Item entities and provides CRUD
 * operations for each. It is an application-level data model — not an entity
 * itself — and does NOT implement IInspectable.
 *
 * @see ADS::Entities::Scene
 * @see ADS::Entities::Character
 * @see ADS::Entities::Item
 */

#include <memory>
#include <string>
#include <vector>

#include "Entities/Scene.h"
#include "Entities/Character.h"
#include "Entities/Item.h"

namespace ADS::Core {

    /**
     * @brief Top-level container for all game entities in a project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Project owns the collections of Scene, Character, and Item instances
     * via std::unique_ptr. Accessors return raw (non-owning) pointers to
     * individual entities. The class is non-copyable because it holds
     * unique ownership of its entities.
     */
    class Project {
    private:
        std::string m_name;                                             ///< Project display name
        std::vector<std::unique_ptr<Entities::Scene>>     m_scenes;    ///< Owned scene collection
        std::vector<std::unique_ptr<Entities::Character>> m_characters; ///< Owned character collection
        std::vector<std::unique_ptr<Entities::Item>>      m_items;     ///< Owned item collection

    public:
        /**
         * @brief Construct a new Project with the given name
         *
         * @param name Human-readable project name
         */
        explicit Project(const std::string& name);

        ~Project() = default;

        // Non-copyable (owns unique_ptrs)
        Project(const Project&) = delete;
        Project& operator=(const Project&) = delete;

        // --- Project metadata ---

        /**
         * @brief Get the project name
         * @return const std::string& Project name
         */
        [[nodiscard]] const std::string& getName() const;

        /**
         * @brief Set the project name
         * @param name New project name
         */
        void setName(const std::string& name);

        // --- Scene CRUD ---

        /**
         * @brief Create and add a new Scene to the project
         *
         * @param id   Unique identifier for the scene
         * @param name Display name for the scene
         * @return Entities::Scene* Non-owning pointer to the newly created scene,
         *         or nullptr if a scene with the same id already exists
         */
        Entities::Scene* addScene(const std::string& id, const std::string& name);

        /**
         * @brief Remove the scene with the given id
         *
         * @param id Unique identifier of the scene to remove
         */
        void removeScene(const std::string& id);

        /**
         * @brief Find a scene by id
         *
         * @param id Unique identifier to search for
         * @return Entities::Scene* Non-owning pointer to the scene, or nullptr if not found
         */
        [[nodiscard]] Entities::Scene* findScene(const std::string& id) const;

        /**
         * @brief Get the full scene collection (read-only)
         * @return const std::vector<std::unique_ptr<Entities::Scene>>& Owned scene vector
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Scene>>& getScenes() const;

        // --- Character CRUD ---

        /**
         * @brief Create and add a new Character to the project
         *
         * @param id   Unique identifier for the character
         * @param name Display name for the character
         * @return Entities::Character* Non-owning pointer to the newly created character,
         *         or nullptr if a character with the same id already exists
         */
        Entities::Character* addCharacter(const std::string& id, const std::string& name);

        /**
         * @brief Remove the character with the given id
         *
         * @param id Unique identifier of the character to remove
         */
        void removeCharacter(const std::string& id);

        /**
         * @brief Find a character by id
         *
         * @param id Unique identifier to search for
         * @return Entities::Character* Non-owning pointer to the character, or nullptr if not found
         */
        [[nodiscard]] Entities::Character* findCharacter(const std::string& id) const;

        /**
         * @brief Get the full character collection (read-only)
         * @return const std::vector<std::unique_ptr<Entities::Character>>& Owned character vector
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Character>>& getCharacters() const;

        // --- Item CRUD ---

        /**
         * @brief Create and add a new Item to the project
         *
         * @param id   Unique identifier for the item
         * @param name Display name for the item
         * @return Entities::Item* Non-owning pointer to the newly created item,
         *         or nullptr if an item with the same id already exists
         */
        Entities::Item* addItem(const std::string& id, const std::string& name);

        /**
         * @brief Remove the item with the given id
         *
         * @param id Unique identifier of the item to remove
         */
        void removeItem(const std::string& id);

        /**
         * @brief Find an item by id
         *
         * @param id Unique identifier to search for
         * @return Entities::Item* Non-owning pointer to the item, or nullptr if not found
         */
        [[nodiscard]] Entities::Item* findItem(const std::string& id) const;

        /**
         * @brief Get the full item collection (read-only)
         * @return const std::vector<std::unique_ptr<Entities::Item>>& Owned item vector
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Item>>& getItems() const;
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_H
