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

#ifndef ADS_DATA_EFFECT_H
#define ADS_DATA_EFFECT_H

/**
 * @file Effect.h
 * @brief Shared damage / heal effect group used by the item DataObject
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstdint>

namespace ADS::Data {

    /**
     * @brief A single damage-or-heal effect an item applies when used.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Mirrors the `effect` schema in
     * docs/core/schemas/common-structures.md (section "Effects
     * (Damages / Heal)"): `applied_to`, `base`, `critical {chance,
     * multiplier}` and `rates {rate, unit}`. Per that doc the fractional
     * `chance`/`multiplier` are stored as integers with the decimal point
     * removed (8.3 -> 83); the divide-by-10 decode is a runtime concern.
     * Only standard C++ types are used — no ImGui or UI framework types.
     */
    struct Effect {
        uint8_t appliedTo      = 0; ///< Target characteristic bitmask (common-structures.md)
        uint8_t base           = 0; ///< Base effect amount
        uint8_t critChance     = 0; ///< Critical chance, x10 (e.g. 83 == 8.3)
        uint8_t critMultiplier = 0; ///< Critical multiplier, x10
        uint8_t rate           = 0; ///< How often the effect ticks
        uint8_t unit           = 0; ///< Unit the rate is expressed in

        /**
         * @brief Check equality with another Effect
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param other The Effect to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const Effect& other) const = default;
    };

} // namespace ADS::Data

#endif // ADS_DATA_EFFECT_H
