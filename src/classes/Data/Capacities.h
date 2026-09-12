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

#ifndef ADS_DATA_CAPACITIES_H
#define ADS_DATA_CAPACITIES_H

/**
 * @file Capacities.h
 * @brief Shared capacity/vitality group used by the character DataObject
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstdint>

namespace ADS::Data {

    /**
     * @brief Numeric resources gating what a character can physically and
     *        mentally do.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Mirrors the `capacities` group in docs/core/schemas/character.md:
     * `load` plus the three `vitalities` (`life`, `stamina`, `sanity`). Each
     * value is stored raw as a byte; the schema's normalisation formula is a
     * presentation concern applied by the game runtime, not here. Only
     * standard C++ types are used — no ImGui or UI framework types.
     */
    struct Capacities {
        uint8_t load    = 0; ///< Carry capacity (character.md: capacities.load)
        uint8_t life    = 0; ///< Health; reaching 0 kills the character
        uint8_t stamina = 0; ///< Physical endurance
        uint8_t sanity  = 0; ///< Mental stability

        /**
         * @brief Check equality with another Capacities
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param other The Capacities to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const Capacities& other) const = default;
    };

} // namespace ADS::Data

#endif // ADS_DATA_CAPACITIES_H
