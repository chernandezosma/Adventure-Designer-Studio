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

#ifndef ADS_CORE_PROJECT_H
#define ADS_CORE_PROJECT_H

/**
 * @file Project.h
 * @brief Top-level application data model that owns all game DataObjects and entities
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * The Project class is the runtime container for a game project. It owns
 * both the DataObject collections (SceneData, CharacterData, ItemData) and
 * the corresponding entity adapters (Scene, Character, Item). Entities hold
 * non-owning pointers into the DataObject collections.
 *
 * DataObjects are the authoritative store for game data and will be
 * serialised/deserialised as the .ads file format. Entities are inspector
 * adapters that are rebuilt whenever the project loads.
 *
 * @see ADS::Data::SceneData
 * @see ADS::Data::CharacterData
 * @see ADS::Data::ItemData
 * @see ADS::Entities::Scene
 * @see ADS::Entities::Character
 * @see ADS::Entities::Item
 */

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Data/SceneData.h"
#include "Data/CharacterData.h"
#include "Data/ItemData.h"
#include "Entities/Scene.h"
#include "Entities/Character.h"
#include "Entities/Item.h"

namespace ADS::Core {

    /**
     * @brief Top-level container for all game DataObjects and entity adapters
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Project owns DataObject collections (via std::unique_ptr) and the
     * corresponding entity adapter collections. DataObject lifetime governs
     * entity lifetime: entities must be destroyed before their DataObjects.
     * The class is non-copyable.
     */
    class Project {
    private:
        std::string m_name;                                                      ///< Project display name
        std::optional<std::filesystem::path> m_filePath;                         ///< Path on disk — empty until first save

        // --- DataObject collections (owned, serialisable) ---
        std::vector<std::unique_ptr<Data::SceneData>>      m_sceneData;          ///< Owned scene DataObjects
        std::vector<std::unique_ptr<Data::CharacterData>>  m_characterData;      ///< Owned character DataObjects
        std::vector<std::unique_ptr<Data::ItemData>>       m_itemData;           ///< Owned item DataObjects

        // --- Entity adapter collections (owned, rebuilt on load) ---
        std::vector<std::unique_ptr<Entities::Scene>>      m_scenes;             ///< Owned scene entity adapters
        std::vector<std::unique_ptr<Entities::Character>>  m_characters;         ///< Owned character entity adapters
        std::vector<std::unique_ptr<Entities::Item>>       m_items;              ///< Owned item entity adapters

    public:
        /**
         * @brief Construct a new Project with the given name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name Human-readable display name for the project
         */
        explicit Project(const std::string& name);

        /**
         * @brief Destroy the Project object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
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
         * @version Mar 2026
         *
         * @return const std::string& Reference to the internal project name string
         */
        [[nodiscard]] const std::string& getName() const;

        /**
         * @brief Set the project name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name New human-readable display name
         */
        void setName(const std::string& name);

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
        [[nodiscard]] bool isSaved() const;

        /**
         * @brief Get the path to the project file on disk
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::filesystem::path& Reference to the stored file path
         * @see isSaved(), setFilePath()
         */
        [[nodiscard]] const std::filesystem::path& getFilePath() const;

        /**
         * @brief Associate a filesystem path with this project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param path Absolute or relative path to the project file
         * @see isSaved(), getFilePath(), clearFilePath()
         */
        void setFilePath(const std::filesystem::path& path);

        /**
         * @brief Remove the associated file path from this project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @see isSaved(), setFilePath()
         */
        void clearFilePath();

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
        Entities::Scene* addScene(const std::string& id, const std::string& name);

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
        void removeScene(const std::string& id);

        /**
         * @brief Find a scene entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Scene* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Scene* findScene(const std::string& id) const;

        /**
         * @brief Get the full scene entity adapter collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Entities::Scene>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Scene>>& getScenes() const;

        /**
         * @brief Get the full scene DataObject collection (read-only, for serialisation)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Data::SceneData>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Data::SceneData>>& getSceneData() const;

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
        Entities::Character* addCharacter(const std::string& id, const std::string& name);

        /**
         * @brief Remove the character with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier of the character to remove
         */
        void removeCharacter(const std::string& id);

        /**
         * @brief Find a character entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Character* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Character* findCharacter(const std::string& id) const;

        /**
         * @brief Get the full character entity adapter collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Entities::Character>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Character>>& getCharacters() const;

        /**
         * @brief Get the full character DataObject collection (read-only, for serialisation)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Data::CharacterData>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Data::CharacterData>>& getCharacterData() const;

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
        Entities::Item* addItem(const std::string& id, const std::string& name);

        /**
         * @brief Remove the item with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier of the item to remove
         */
        void removeItem(const std::string& id);

        /**
         * @brief Find an item entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Item* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Item* findItem(const std::string& id) const;

        /**
         * @brief Get the full item entity adapter collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Entities::Item>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::Item>>& getItems() const;

        /**
         * @brief Get the full item DataObject collection (read-only, for serialisation)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::unique_ptr<Data::ItemData>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Data::ItemData>>& getItemData() const;
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_H
