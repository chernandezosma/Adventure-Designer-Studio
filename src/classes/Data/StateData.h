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

#ifndef ADS_DATA_STATE_DATA_H
#define ADS_DATA_STATE_DATA_H

/**
 * @file StateData.h
 * @brief Pure data class for a state-catalog entry — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include <optional>
#include <string>

#include "BaseData.h"
#include "Descriptions.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for one entry in the shared State catalog.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * A State is an individually authored record — id, name, and the text
     * shown to the player while it's active — that any entity (Scene today,
     * Item/Character later) can reference as the state it starts in. States
     * are linked into chains via next(): following next() repeatedly walks
     * the chain until std::nullopt marks its end. Only standard C++ types
     * are used — no ImGui or UI framework types appear here. This class is
     * owned by Core::Project; Entities::State reads/writes through a
     * non-owning pointer to this object.
     */
    class StateData : public BaseData<ADS::Types::StateTag> {
    public:
        /**
         * @brief Get the sensory descriptions shown while this state is active
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const Descriptions& LexEngine text-id references for this state
         */
        [[nodiscard]] const Descriptions& getDescriptions() const { return m_descriptions; }

        /**
         * @brief Set the sensory descriptions shown while this state is active
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Descriptions& descriptions) { m_descriptions = descriptions; }

        /**
         * @brief Get the author-typed draft text for this state's descriptions
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
        [[nodiscard]] const DescriptionTexts& getDescriptionTexts() const { return m_descriptionTexts; }

        /**
         * @brief Set the author-typed draft text for this state's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
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
         * @brief Get the next state in this state's chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Next state id, or
         *         std::nullopt if this is the last state in its chain
         */
        [[nodiscard]] const std::optional<ADS::Types::StateId>& getNext() const { return m_next; }

        /**
         * @brief Set the next state in this state's chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param next The new next-state id, or std::nullopt to mark this as
         *             the last state in its chain
         */
        void setNext(const std::optional<ADS::Types::StateId>& next) { m_next = next; }

    private:
        Descriptions     m_descriptions;     ///< LexEngine text-id references shown while active
        DescriptionTexts m_descriptionTexts; ///< IDE-only draft text backing m_descriptions
        LocalizedText    m_nameTexts;        ///< IDE-only per-language name overrides (.trn only)
        std::optional<ADS::Types::StateId> m_next; ///< Next state in this chain, or std::nullopt if last
    };

} // namespace ADS::Data

#endif // ADS_DATA_STATE_DATA_H
