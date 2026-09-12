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
 * @file SceneData.cpp
 * @brief Implementation of the SceneData DataObject
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "SceneData.h"

namespace ADS::Data {

    /**
     * @brief Get the sensory descriptions of the scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const Descriptions& LexEngine text-id references for this scene
     */
    const Descriptions& SceneData::getDescriptions() const {
        return m_descriptions;
    }

    /**
     * @brief Set the sensory descriptions of the scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param descriptions The new descriptions
     */
    void SceneData::setDescriptions(const Descriptions& descriptions) {
        m_descriptions = descriptions;
    }

    /**
     * @brief Get the author-typed draft text for the scene's descriptions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * IDE-only convenience, not part of the .ads file schema — see
     * DescriptionTexts. Separate from getDescriptions(), which holds
     * the (not-yet-compiled) LexEngine entry ids.
     *
     * @return const DescriptionTexts& Draft description text
     */
    const DescriptionTexts& SceneData::getDescriptionTexts() const {
        return m_descriptionTexts;
    }

    /**
     * @brief Set the author-typed draft text for the scene's descriptions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param texts The new draft description text
     */
    void SceneData::setDescriptionTexts(const DescriptionTexts& texts) {
        m_descriptionTexts = texts;
    }

    /**
     * @brief Get the filename or base64-encoded image for this scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::string& Image filename or base64 payload
     */
    const std::string& SceneData::getImage() const {
        return m_image;
    }

    /**
     * @brief Set the filename or base64-encoded image for this scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param image The new image filename or base64 payload
     */
    void SceneData::setImage(const std::string& image) {
        m_image = image;
    }

    /**
     * @brief Get the scene's current state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Author-set reference to a state in the project's shared State
     * catalog — see ADS::Data::StateData. std::nullopt means no state
     * applies to this scene. Unlike a State's own "next" or a
     * StateChain's own "head" (which must stay "free" to keep a chain's
     * internal links well-formed — see Project::getFreeStates()), a
     * Scene may freely reference any state, including one another scene
     * also references.
     *
     * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
     */
    const std::optional<ADS::Types::StateId>& SceneData::getState() const {
        return m_state;
    }

    /**
     * @brief Set the scene's current state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param state The new state id, or std::nullopt to clear it
     */
    void SceneData::setState(const std::optional<ADS::Types::StateId>& state) {
        m_state = state;
    }

    /**
     * @brief Get the scene's directional exits
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const Exits& The ten named directional exits
     */
    const SceneData::Exits& SceneData::getExits() const {
        return m_exits;
    }

    /**
     * @brief Set the scene's directional exits
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param exits The new exits
     */
    void SceneData::setExits(const Exits& exits) {
        m_exits = exits;
    }

    /**
     * @brief Get the IDs of items present in this scene at load time
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * This is the IDE authoring-time representation of scene.md's
     * "items" field. scene.md describes the compiled/runtime encoding
     * as a uint16_t pointer into a packed item table — that describes
     * the compiled output, not this authoring model, the same way
     * affordances/flags stay plain uint8_t here rather than bit-level
     * structs.
     *
     * @return const std::vector<ADS::Types::ObjectId>& Present item IDs
     */
    const std::vector<ADS::Types::ObjectId>& SceneData::getPresentItemIds() const {
        return m_presentItemIds;
    }

    /**
     * @brief Set the IDs of items present in this scene at load time
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param ids The new present item ID vector
     */
    void SceneData::setPresentItemIds(const std::vector<ADS::Types::ObjectId>& ids) {
        m_presentItemIds = ids;
    }

    /**
     * @brief Get the scene's trigger map
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Maps a global trigger id (see scene.md's Global triggers table,
     * e.g. 0x01 = on_enter) to an ordered array of EventIds.
     *
     * @return const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Trigger map
     */
    const std::map<uint8_t, std::vector<ADS::Types::EventId>>& SceneData::getTriggers() const {
        return m_triggers;
    }

    /**
     * @brief Replace the scene's entire trigger map
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param triggers The new trigger map
     */
    void SceneData::setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>>& triggers) {
        m_triggers = triggers;
    }

    /**
     * @brief Append an EventId handler for a global trigger
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Appends to the existing array for @p eventId if one already
     * exists, otherwise creates it. Never overwrites — scene.md
     * requires the same trigger key not repeat, so multiple handlers
     * for one trigger always share a single array entry.
     *
     * @param eventId Global trigger id (e.g. 0x01 = on_enter)
     * @param handler EventId to append to that trigger's handler list
     */
    void SceneData::addTrigger(uint8_t eventId, ADS::Types::EventId handler) {
        m_triggers[eventId].push_back(handler);
    }

    /**
     * @brief Get the scene's affordances
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Author-named affordances and the trigger names each one fires —
     * see Data::Affordance. Unrelated to getTriggers()'s fixed
     * EventId-keyed trigger map.
     *
     * @return const std::vector<Affordance>& The scene's affordances
     */
    const std::vector<Affordance>& SceneData::getAffordances() const {
        return m_affordances;
    }

    /**
     * @brief Set the scene's affordances
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param affordances The new affordance list
     */
    void SceneData::setAffordances(const std::vector<Affordance>& affordances) {
        m_affordances = affordances;
    }

    /**
     * @brief Check whether this is the game's starting scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * IDE-only convenience field — not part of the .ads file schema.
     * The authoritative starting scene is Game.starting-scene.
     *
     * @return bool True if this is the starting scene
     */
    bool SceneData::isStartScene() const {
        return m_isStartScene;
    }

    /**
     * @brief Set whether this is the game's starting scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * IDE-only convenience field — not part of the .ads file schema.
     *
     * @param isStart True to mark this as the starting scene
     */
    void SceneData::setStartScene(bool isStart) {
        m_isStartScene = isStart;
    }

} // namespace ADS::Data
