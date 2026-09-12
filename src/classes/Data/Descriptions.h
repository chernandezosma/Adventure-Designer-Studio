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

#ifndef ADS_DATA_DESCRIPTIONS_H
#define ADS_DATA_DESCRIPTIONS_H

/**
 * @file Descriptions.h
 * @brief Shared sensory-description group used by scene/item/character DataObjects
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace ADS::Data {

    /**
     * @brief Per-language text storage for a translatable field.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Keyed by BCP-47/POSIX language code (e.g. "es_ES"), matching
     * ADS::Constants::Languages' code format and LexEngine::LanguageCode.
     * Deliberately a separate type from Inspector::LocalizedText — Data/
     * never depends on Inspector/; entities convert between the two at
     * their get/setPropertyValue boundary.
     */
    using LocalizedText = std::map<std::string, std::string>;

    /**
     * @brief Group of LexEngine text references describing an entity.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Implements the Descriptions shape documented in
     * docs/core/schemas/common-structures.md (anchor "decriptions-definition"),
     * shared by scene/item/character schemas. Each field is a placeholder
     * LexEngine entry id (uint32_t) — no LexEngine resolution logic exists
     * yet, this only carries the correctly-shaped reference. "long" is a
     * C++ keyword, so the schema's "long" field is named longText here.
     */
    struct Descriptions {
        uint32_t normal = 0;               ///< Required — shown on revisit or LOOK
        uint32_t longText = 0;              ///< Required — shown on first visit
        std::optional<uint32_t> odor;       ///< Optional — omit if no ambient smell
        std::optional<uint32_t> sound;      ///< Optional — omit if no ambient sound

        /**
         * @brief Check equality with another Descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param other The Descriptions to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const Descriptions& other) const = default;
    };

    /**
     * @brief Author-typed source text backing a Descriptions group.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * IDE-only convenience data, not part of the .ads file schema. Holds
     * the human-authored draft text for each Descriptions slot. A future
     * LexEngine integration will compile this text into the LexEngine
     * entry ids stored on Descriptions — that compilation step does not
     * exist yet, so the two structs are deliberately separate: editing
     * this text does not by itself update the corresponding Descriptions
     * ids.
     */
    struct DescriptionTexts {
        LocalizedText normal;   ///< Draft text for the required "normal" slot, per language
        LocalizedText longText; ///< Draft text for the required "long" slot, per language
        LocalizedText odor;     ///< Draft text for the optional "odor" slot, per language
        LocalizedText sound;    ///< Draft text for the optional "sound" slot, per language

        /**
         * @brief Check equality with another DescriptionTexts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param other The DescriptionTexts to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const DescriptionTexts& other) const = default;
    };

} // namespace ADS::Data

#endif // ADS_DATA_DESCRIPTIONS_H