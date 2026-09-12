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

#ifndef ADS_DATA_CHARACTER_DATA_H
#define ADS_DATA_CHARACTER_DATA_H

/**
 * @file CharacterData.h
 * @brief Pure data class for a game character — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Affordance.h"
#include "BaseData.h"
#include "Capacities.h"
#include "Descriptions.h"
#include "Types/Color.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a game character (player or NPC).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Holds all persistent data for a character. Only standard C++ types and
     * ADS::Types are used — no ImGui or UI framework types appear here.
     * This class is owned by Core::Project; Entities::Character reads/writes
     * through a non-owning pointer to this object. Field set follows
     * docs/core/schemas/character.md.
     */
    class CharacterData : public BaseData<ADS::Types::CharacterTag> {
    public:
        /**
         * @brief Get the character's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Descriptions& LexEngine text-id references for this character
         */
        [[nodiscard]] const Descriptions& getDescriptions() const { return m_descriptions; }

        /**
         * @brief Set the character's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Descriptions& descriptions) { m_descriptions = descriptions; }

        /**
         * @brief Get the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const DescriptionTexts& Draft description text, per language
         */
        [[nodiscard]] const DescriptionTexts& getDescriptionTexts() const { return m_descriptionTexts; }

        /**
         * @brief Set the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const DescriptionTexts& texts) { m_descriptionTexts = texts; }

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
         * @brief Check whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the player-controlled character
         */
        [[nodiscard]] bool isPlayer() const { return m_isPlayer; }

        /**
         * @brief Set whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param isPlayer True to mark this as the player character
         */
        void setPlayer(bool isPlayer) { m_isPlayer = isPlayer; }

        /**
         * @brief Get the character's capacities (load + life/stamina/sanity)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Capacities& Current capacity values
         */
        [[nodiscard]] const Capacities& getCapacities() const { return m_capacities; }

        /**
         * @brief Set the character's capacities (load + life/stamina/sanity)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param capacities The new capacity values
         */
        void setCapacities(const Capacities& capacities) { m_capacities = capacities; }

        /**
         * @brief Get the character's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Author-set reference into the project's shared State catalog
         * (docs/core/schemas/character.md#States). std::nullopt means no
         * state modifies this character's capacities. Same treatment as
         * SceneData/ItemData: a plain reference, not restricted to "free"
         * states — more than one entity may reference the same state.
         *
         * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
         */
        [[nodiscard]] const std::optional<ADS::Types::StateId>& getState() const { return m_state; }

        /**
         * @brief Set the character's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param state The new state id, or std::nullopt to clear it
         */
        void setState(const std::optional<ADS::Types::StateId>& state) { m_state = state; }

        /**
         * @brief Get the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Dialog text color
         */
        [[nodiscard]] const ADS::Types::Color& getDialogColor() const { return m_dialogColor; }

        /**
         * @brief Set the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param color The new dialog text color
         */
        void setDialogColor(const ADS::Types::Color& color) { m_dialogColor = color; }

        /**
         * @brief Get the path to the character's main (full-body) image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * character.md's `image.image` slot — filename or encoded string.
         *
         * @return const std::string& File path to the main image
         */
        [[nodiscard]] const std::string& getImagePath() const { return m_imagePath; }

        /**
         * @brief Set the path to the character's main (full-body) image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param path The new main image file path
         */
        void setImagePath(const std::string& path) { m_imagePath = path; }

        /**
         * @brief Get the path to the character's avatar image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * character.md's `image.avatar` slot. "Avatar" is the schema's name
         * for the character portrait shown next to dialog.
         *
         * @return const std::string& File path to the avatar image
         */
        [[nodiscard]] const std::string& getAvatarPath() const { return m_avatarPath; }

        /**
         * @brief Set the path to the character's avatar image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param path The new avatar image file path
         */
        void setAvatarPath(const std::string& path) { m_avatarPath = path; }

        /**
         * @brief Get the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::optional<ADS::Types::SceneId>& Starting scene ID,
         *         or std::nullopt if the character has no starting scene yet
         */
        [[nodiscard]] const std::optional<ADS::Types::SceneId>& getInitialSceneId() const { return m_initialSceneId; }

        /**
         * @brief Set the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sceneId The new starting scene ID, or std::nullopt to clear it
         */
        void setInitialSceneId(const std::optional<ADS::Types::SceneId>& sceneId) { m_initialSceneId = sceneId; }

        /**
         * @brief Get the character's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Author-named affordances and the trigger names each one fires —
         * see Data::Affordance.
         *
         * @return const std::vector<Affordance>& The character's affordances
         */
        [[nodiscard]] const std::vector<Affordance>& getAffordances() const { return m_affordances; }

        /**
         * @brief Set the character's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param affordances The new affordance list
         */
        void setAffordances(const std::vector<Affordance>& affordances) { m_affordances = affordances; }

        /**
         * @brief Get the character's trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Maps a character-applicable global trigger id (see
         * common-structures.md's Global triggers table, e.g. 0x08 =
         * on_talk) to an ordered array of EventIds. Independent of
         * Scene's own trigger map — same shape, separate instance.
         *
         * @return const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Trigger map
         */
        [[nodiscard]] const std::map<uint8_t, std::vector<ADS::Types::EventId>>& getTriggers() const { return m_triggers; }

        /**
         * @brief Replace the character's entire trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param triggers The new trigger map
         */
        void setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>>& triggers) { m_triggers = triggers; }

        /**
         * @brief Append an EventId handler for a global trigger
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Appends to the existing array for @p eventId if one already
         * exists, otherwise creates it. Never overwrites.
         *
         * @param eventId Global trigger id (e.g. 0x08 = on_talk)
         * @param handler EventId to append to that trigger's handler list
         */
        void addTrigger(uint8_t eventId, ADS::Types::EventId handler) { m_triggers[eventId].push_back(handler); }

    private:
        Descriptions      m_descriptions;                            ///< LexEngine text-id references (character.md descriptions)
        DescriptionTexts  m_descriptionTexts;                        ///< IDE-only draft text backing m_descriptions
        LocalizedText     m_nameTexts;                               ///< IDE-only per-language name overrides (.trn only)
        bool              m_isPlayer   = false;                      ///< Whether this is the player character
        Capacities        m_capacities;                              ///< load + vitalities (life/stamina/sanity)
        std::optional<ADS::Types::StateId> m_state;                  ///< Author-set reference into the project's State catalog
        ADS::Types::Color m_dialogColor = {1.0f, 1.0f, 1.0f, 1.0f};  ///< Color used for dialog text
        std::string       m_imagePath;                              ///< character.md image.image
        std::string       m_avatarPath;                            ///< character.md image.avatar
        std::optional<ADS::Types::SceneId> m_initialSceneId;         ///< ID of the scene where this character starts
        std::vector<Affordance> m_affordances;                       ///< Author-named affordances and their triggers
        std::map<uint8_t, std::vector<ADS::Types::EventId>> m_triggers; ///< Global trigger id -> EventId handlers
    };

} // namespace ADS::Data

#endif // ADS_DATA_CHARACTER_DATA_H
