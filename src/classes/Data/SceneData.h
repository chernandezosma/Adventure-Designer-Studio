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

#ifndef ADS_DATA_SCENE_DATA_H
#define ADS_DATA_SCENE_DATA_H

/**
 * @file SceneData.h
 * @brief Pure data class for a game scene — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Declaration only — see SceneData.cpp for definitions. SceneData is the
 * first Data/ class to split declaration from implementation: unlike the
 * other DataObjects, it now carries real helper logic (Exits lookups,
 * trigger-array insertion) beyond one-line accessors. This sets the
 * precedent ItemData/CharacterData should follow when they get their own
 * schema-conformance pass.
 */

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Affordance.h"
#include "BaseData.h"
#include "Descriptions.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a game scene.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Holds all persistent data for a scene, matching the schema documented
     * in docs/core/schemas/scene.md. Only standard C++ types and ADS::Types
     * are used — no ImGui or UI framework types appear here. This class is
     * owned by Core::Project and is the authoritative source of truth for
     * the scene; Entities::Scene reads/writes through a non-owning pointer
     * to this object.
     */
    class SceneData : public BaseData<ADS::Types::SceneTag> {
    public:
        /**
         * @brief Fixed set of the ten named directional exits of a scene.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Each direction holds the destination SceneId, or std::nullopt if
         * no passage exists in that direction. Nested inside SceneData
         * since it has no meaning outside a scene.
         */
        struct Exits {
            std::optional<ADS::Types::SceneId> north;     ///< North exit destination, if any
            std::optional<ADS::Types::SceneId> south;     ///< South exit destination, if any
            std::optional<ADS::Types::SceneId> east;      ///< East exit destination, if any
            std::optional<ADS::Types::SceneId> west;      ///< West exit destination, if any
            std::optional<ADS::Types::SceneId> northeast; ///< Northeast exit destination, if any
            std::optional<ADS::Types::SceneId> northwest; ///< Northwest exit destination, if any
            std::optional<ADS::Types::SceneId> southeast; ///< Southeast exit destination, if any
            std::optional<ADS::Types::SceneId> southwest; ///< Southwest exit destination, if any
            std::optional<ADS::Types::SceneId> up;        ///< Up exit destination, if any
            std::optional<ADS::Types::SceneId> down;      ///< Down exit destination, if any

            /**
             * @brief Check equality with another Exits set
             *
             * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
             * @version Mar 2026
             *
             * @param other The Exits to compare against
             * @return bool True if every direction is equal
             */
            bool operator==(const Exits& other) const = default;
        };

        /**
         * @brief Get the sensory descriptions of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const Descriptions& LexEngine text-id references for this scene
         */
        [[nodiscard]] const Descriptions& getDescriptions() const;

        /**
         * @brief Set the sensory descriptions of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Descriptions& descriptions);

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
        [[nodiscard]] const DescriptionTexts& getDescriptionTexts() const;

        /**
         * @brief Set the author-typed draft text for the scene's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const DescriptionTexts& texts);

        /**
         * @brief Get the per-language name overrides.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * IDE-only, not in `.ads` — persisted to `.trn`. The default-language
         * name lives in BaseData::m_name; this map holds the other languages.
         *
         * @return const LocalizedText& langCode -> translated name
         */
        [[nodiscard]] const LocalizedText& getNameTexts() const { return m_nameTexts; }

        /**
         * @brief Replace the per-language name overrides.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param texts New langCode -> name map
         */
        void setNameTexts(const LocalizedText& texts) { m_nameTexts = texts; }

        /**
         * @brief Get the filename or base64-encoded image for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Image filename or base64 payload
         */
        [[nodiscard]] const std::string& getImage() const;

        /**
         * @brief Set the filename or base64-encoded image for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param image The new image filename or base64 payload
         */
        void setImage(const std::string& image);

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
        [[nodiscard]] const std::optional<ADS::Types::StateId>& getState() const;

        /**
         * @brief Set the scene's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param state The new state id, or std::nullopt to clear it
         */
        void setState(const std::optional<ADS::Types::StateId>& state);

        /**
         * @brief Get the scene's directional exits
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const Exits& The ten named directional exits
         */
        [[nodiscard]] const Exits& getExits() const;

        /**
         * @brief Set the scene's directional exits
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param exits The new exits
         */
        void setExits(const Exits& exits);

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
        [[nodiscard]] const std::vector<ADS::Types::ObjectId>& getPresentItemIds() const;

        /**
         * @brief Set the IDs of items present in this scene at load time
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param ids The new present item ID vector
         */
        void setPresentItemIds(const std::vector<ADS::Types::ObjectId>& ids);

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
        [[nodiscard]] const std::map<uint8_t, std::vector<ADS::Types::EventId>>& getTriggers() const;

        /**
         * @brief Replace the scene's entire trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param triggers The new trigger map
         */
        void setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>>& triggers);

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
        void addTrigger(uint8_t eventId, ADS::Types::EventId handler);

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
        [[nodiscard]] const std::vector<Affordance>& getAffordances() const;

        /**
         * @brief Set the scene's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param affordances The new affordance list
         */
        void setAffordances(const std::vector<Affordance>& affordances);

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
        [[nodiscard]] bool isStartScene() const;

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
        void setStartScene(bool isStart);

    private:
        // --- Schema-defined fields (docs/core/schemas/scene.md) ---
        Descriptions m_descriptions;    ///< LexEngine text-id references (scene.md descriptions)
        // IDE-only draft text backing m_descriptions — see DescriptionTexts
        DescriptionTexts m_descriptionTexts; ///< IDE-only draft text backing m_descriptions
        LocalizedText    m_nameTexts;   ///< IDE-only per-language name overrides (.trn only)
        std::string  m_image;           ///< Image filename or base64 payload
        std::optional<ADS::Types::StateId> m_state; ///< Author-set reference into the project's State catalog
        Exits        m_exits;           ///< The ten named directional exits
        std::vector<ADS::Types::ObjectId> m_presentItemIds; ///< Item IDs present in this scene at load time
        std::map<uint8_t, std::vector<ADS::Types::EventId>> m_triggers; ///< Global trigger id -> EventId handlers
        std::vector<Affordance> m_affordances; ///< Author-named affordances and their triggers

        // --- IDE convenience, not part of the .ads file schema ---
        bool m_isStartScene = false; ///< Whether this is the game's starting scene
    };

} // namespace ADS::Data

#endif // ADS_DATA_SCENE_DATA_H