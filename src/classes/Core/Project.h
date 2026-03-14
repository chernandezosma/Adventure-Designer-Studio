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

#include <filesystem>
#include <memory>
#include <optional>
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
        std::optional<std::filesystem::path> m_filePath;               ///< Path on disk — empty until first save
        std::vector<std::unique_ptr<Entities::Scene>>     m_scenes;    ///< Owned scene collection
        std::vector<std::unique_ptr<Entities::Character>> m_characters; ///< Owned character collection
        std::vector<std::unique_ptr<Entities::Item>>      m_items;     ///< Owned item collection

    public:
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
        explicit Project(const std::string& name);

        /**
         * @brief Destroy the Project object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Default destructor. All entity collections are automatically released
         * through their unique_ptr destructors.
         */
        ~Project() = default;

        // Non-copyable (owns unique_ptrs)
        Project(const Project&) = delete;
        Project& operator=(const Project&) = delete;

        // --- Project metadata ---

        /**
         * @brief Get the project name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Returns the human-readable display name of the project.
         *
         * @return const std::string& Reference to the internal project name string
         */
        [[nodiscard]] const std::string& getName() const;

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
        void setName(const std::string& name);

        // --- File path ---

        /**
         * @brief Check whether this project has been saved to disk
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Returns true once a file path has been associated with the project
         * via setFilePath(). A brand-new project that has never been saved
         * returns false, and the first save should trigger a "Save As" dialog.
         *
         * @return bool True if a file path is set, false for an unsaved project
         * @see setFilePath()
         */
        [[nodiscard]] bool isSaved() const;

        /**
         * @brief Get the path to the project file on disk
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Returns the filesystem path where the project was last saved or
         * loaded from. Call isSaved() before this method — the behaviour is
         * undefined if no path has been set yet.
         *
         * @return const std::filesystem::path& Reference to the stored file path
         * @see isSaved(), setFilePath()
         */
        [[nodiscard]] const std::filesystem::path& getFilePath() const;

        /**
         * @brief Associate a filesystem path with this project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Stores the path where the project was saved or loaded from.
         * After this call isSaved() returns true and subsequent plain "Save"
         * operations can overwrite the same file without showing a dialog.
         *
         * @param path Absolute or relative path to the project file
         * @see isSaved(), getFilePath(), clearFilePath()
         */
        void setFilePath(const std::filesystem::path& path);

        /**
         * @brief Remove the associated file path from this project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Resets the project to an "unsaved" state by clearing the stored path.
         * After this call isSaved() returns false and the next save will prompt
         * the user to choose a location. Useful when duplicating a project or
         * resetting it to a clean state.
         *
         * @see isSaved(), setFilePath()
         */
        void clearFilePath();

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
         * @return Entities::Scene* Non-owning pointer to the newly created scene,
         *         or nullptr if a scene with the same id already exists
         */
        Entities::Scene* addScene(const std::string& id, const std::string& name);

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
        void removeScene(const std::string& id);

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
         * @return Entities::Scene* Non-owning pointer to the scene,
         *         or nullptr if not found
         */
        [[nodiscard]] Entities::Scene* findScene(const std::string& id) const;

        /**
         * @brief Get the full scene collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides read-only access to the owned scene vector for iteration.
         *
         * @return const std::vector<std::unique_ptr<Entities::Scene>>&
         *         Reference to the owned scene collection
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Scene>>& getScenes() const;

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
         * @return Entities::Character* Non-owning pointer to the newly created
         *         character, or nullptr if a character with the same id already exists
         */
        Entities::Character* addCharacter(const std::string& id, const std::string& name);

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
        void removeCharacter(const std::string& id);

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
         * @return Entities::Character* Non-owning pointer to the character,
         *         or nullptr if not found
         */
        [[nodiscard]] Entities::Character* findCharacter(const std::string& id) const;

        /**
         * @brief Get the full character collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides read-only access to the owned character vector for iteration.
         *
         * @return const std::vector<std::unique_ptr<Entities::Character>>&
         *         Reference to the owned character collection
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Character>>& getCharacters() const;

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
         * @return Entities::Item* Non-owning pointer to the newly created item,
         *         or nullptr if an item with the same id already exists
         */
        Entities::Item* addItem(const std::string& id, const std::string& name);

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
        void removeItem(const std::string& id);

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
         * @return Entities::Item* Non-owning pointer to the item,
         *         or nullptr if not found
         */
        [[nodiscard]] Entities::Item* findItem(const std::string& id) const;

        /**
         * @brief Get the full item collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides read-only access to the owned item vector for iteration.
         *
         * @return const std::vector<std::unique_ptr<Entities::Item>>&
         *         Reference to the owned item collection
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Item>>& getItems() const;
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_H
