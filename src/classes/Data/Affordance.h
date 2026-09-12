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

#ifndef ADS_DATA_AFFORDANCE_H
#define ADS_DATA_AFFORDANCE_H

/**
 * @file Affordance.h
 * @brief An author-named affordance and the triggers it fires
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 */

#include <string>
#include <vector>

namespace ADS::Data {

    /**
     * @brief One affordance an item, scene, or character exposes to the player.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Replaces the old fixed bitmap (see item.md's former "Affordances
     * definitions" table): rather than picking from a closed set of bits,
     * the author names the affordance (e.g. "Takeable") and lists the
     * trigger names it fires (e.g. "on_pickup", "on_drop"). Both are free
     * text — the author is responsible for defining and later resolving
     * them; this struct only stores the pairing. Only standard C++ types
     * are used — no ImGui or UI framework types.
     */
    struct Affordance {
        std::string name;                  ///< Affordance name, e.g. "Takeable" (<=128 chars)
        std::vector<std::string> triggers; ///< Trigger names it fires, e.g. {"on_pickup", "on_drop"}

        /// Translation key this entry was picked from (e.g. "ITEM.AFF_OPENABLE"),
        /// or empty for a custom-typed name. When set, the inspector shows
        /// translate(presetKey) live instead of the stored `name`, so the
        /// display re-translates on a language switch; `name` is still kept
        /// in sync as the last-resolved text for readability in raw JSON and
        /// as a fallback if the key is ever unresolvable. Hand-editing the
        /// name in the inspector clears this — see AffordanceListEditor.
        std::string presetKey;

        /**
         * @brief Check equality with another Affordance
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param other The Affordance to compare against
         * @return bool True if name, triggers, and presetKey are equal
         */
        bool operator==(const Affordance& other) const = default;
    };

} // namespace ADS::Data

#endif // ADS_DATA_AFFORDANCE_H
