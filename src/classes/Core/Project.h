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
 * @see ADS::LexEngine::LexEngine
 */

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Data/SceneData.h"
#include "Data/CharacterData.h"
#include "Data/GameData.h"
#include "Data/ItemData.h"
#include "Data/StateData.h"
#include "Data/StateChainData.h"
#include "Entities/Scene.h"
#include "Entities/Character.h"
#include "Entities/Item.h"
#include "Entities/State.h"
#include "Entities/StateChain.h"
#include "LexEngine/LexEngine.h"
#include "Types/Id.h"

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
        std::optional<std::filesystem::path> m_filePath;                         ///< Path on disk — empty until first save
        Data::GameData m_gameData;                                               ///< Project-wide game settings (game.md); m_gameData.title IS the project name

        // --- DataObject collections (owned, serialisable) ---
        std::vector<std::unique_ptr<Data::SceneData>>      m_sceneData;          ///< Owned scene DataObjects
        std::vector<std::unique_ptr<Data::CharacterData>>  m_characterData;      ///< Owned character DataObjects
        std::vector<std::unique_ptr<Data::ItemData>>       m_itemData;           ///< Owned item DataObjects
        std::vector<std::unique_ptr<Data::StateData>>      m_stateData;          ///< Owned state-catalog DataObjects
        std::vector<std::unique_ptr<Data::StateChainData>> m_chainData;          ///< Owned state-chain DataObjects

        // --- Entity adapter collections (owned, rebuilt on load) ---
        std::vector<std::unique_ptr<Entities::Scene>>      m_scenes;             ///< Owned scene entity adapters
        std::vector<std::unique_ptr<Entities::Character>>  m_characters;         ///< Owned character entity adapters
        std::vector<std::unique_ptr<Entities::Item>>       m_items;              ///< Owned item entity adapters
        std::vector<std::unique_ptr<Entities::State>>      m_states;             ///< Owned state-catalog entity adapters
        std::vector<std::unique_ptr<Entities::StateChain>> m_chains;             ///< Owned state-chain entity adapters

        // --- LexEngine (vocabulary compiler subsystem) ---
        // Default-initialised here, independent of the constructor's `name`
        // parameter — the project/game display name is metadata, not
        // vocabulary, and is never fed into the LexEngine.
        std::unique_ptr<LexEngine::LexEngine> m_lexEngine = std::make_unique<LexEngine::LexEngine>(); ///< Owned LexEngine — no ImGui dependency, no entity adapter

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
        [[nodiscard]] const std::string& getName() const;

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
        Entities::Scene* addScene(ADS::Types::SceneId id, const std::string& name);

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
        void removeScene(ADS::Types::SceneId id);

        /**
         * @brief Find a scene entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Scene* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Scene* findScene(ADS::Types::SceneId id) const;

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
        Entities::Character* addCharacter(ADS::Types::CharacterId id, const std::string& name);

        /**
         * @brief Remove the character with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier of the character to remove
         */
        void removeCharacter(ADS::Types::CharacterId id);

        /**
         * @brief Find a character entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Character* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Character* findCharacter(ADS::Types::CharacterId id) const;

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
        Entities::Item* addItem(ADS::Types::ObjectId id, const std::string& name);

        /**
         * @brief Remove the item with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier of the item to remove
         */
        void removeItem(ADS::Types::ObjectId id);

        /**
         * @brief Find an item entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::Item* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::Item* findItem(ADS::Types::ObjectId id) const;

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
        Entities::State* addState(ADS::Types::StateId id, const std::string& name);

        /**
         * @brief Remove the state with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param id Unique identifier of the state to remove
         */
        void removeState(ADS::Types::StateId id);

        /**
         * @brief Find a state entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::State* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::State* findState(ADS::Types::StateId id) const;

        /**
         * @brief Get the full state entity adapter collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::vector<std::unique_ptr<Entities::State>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::State>>& getStates() const;

        /**
         * @brief Get the full state DataObject collection (read-only, for serialisation)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::vector<std::unique_ptr<Data::StateData>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Data::StateData>>& getStateData() const;

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
        [[nodiscard]] std::vector<Entities::State*> getFreeStates(
            std::optional<ADS::Types::StateId> excludeSelf = std::nullopt,
            std::optional<ADS::Types::StateId> keepEvenIfUsed = std::nullopt
        ) const;

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
        Entities::StateChain* addChain(ADS::Types::ChainId id, const std::string& name);

        /**
         * @brief Remove the chain with the given id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param id Unique identifier of the chain to remove
         */
        void removeChain(ADS::Types::ChainId id);

        /**
         * @brief Find a chain entity adapter by id
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param id Unique identifier to search for
         * @return Entities::StateChain* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] Entities::StateChain* findChain(ADS::Types::ChainId id) const;

        /**
         * @brief Get the full chain entity adapter collection (read-only)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::vector<std::unique_ptr<Entities::StateChain>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Entities::StateChain>>& getChains() const;

        /**
         * @brief Get the full chain DataObject collection (read-only, for serialisation)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::vector<std::unique_ptr<Data::StateChainData>>&
         */
        [[nodiscard]] const std::vector<std::unique_ptr<Data::StateChainData>>& getChainData() const;

        // --- ID allocation ---

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
        [[nodiscard]] ADS::Types::SceneId nextSceneId() const;

        /**
         * @brief Smallest unused character id in this project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return ADS::Types::CharacterId The id to give the next new character
         * @see nextSceneId()
         */
        [[nodiscard]] ADS::Types::CharacterId nextCharacterId() const;

        /**
         * @brief Smallest unused item id in this project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return ADS::Types::ObjectId The id to give the next new item
         * @see nextSceneId()
         */
        [[nodiscard]] ADS::Types::ObjectId nextItemId() const;

        /**
         * @brief Smallest unused state id in this project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return ADS::Types::StateId The id to give the next new state
         * @see nextSceneId()
         */
        [[nodiscard]] ADS::Types::StateId nextStateId() const;

        /**
         * @brief Smallest unused state-chain id in this project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return ADS::Types::ChainId The id to give the next new chain
         * @see nextSceneId()
         */
        [[nodiscard]] ADS::Types::ChainId nextChainId() const;

        // --- Duplication ---

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
        Entities::Scene* duplicateScene(ADS::Types::SceneId sourceId,
                                        const std::string& newName);

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
        Entities::Character* duplicateCharacter(ADS::Types::CharacterId sourceId,
                                                const std::string& newName);

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
        Entities::Item* duplicateItem(ADS::Types::ObjectId sourceId,
                                      const std::string& newName);

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
        Entities::State* duplicateState(ADS::Types::StateId sourceId,
                                        const std::string& newName);

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
        [[nodiscard]] LexEngine::LexEngine& getLexEngine() const;

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
        [[nodiscard]] const Data::GameData& getGameData() const;

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
        [[nodiscard]] Data::GameData& getGameData();
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_H
