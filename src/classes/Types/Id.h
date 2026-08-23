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

#ifndef ADS_TYPES_ID_H
#define ADS_TYPES_ID_H

/**
 * @file Id.h
 * @brief Phantom-typed entity identifier, shared by all DataObjects
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Implements the Id<Tag> pattern documented in
 * docs/core/schemas/common-structures.md. This type has no dependency on
 * ImGui, Data, or Entities and is safe for use anywhere, following the same
 * precedent as ADS::Types::Color. Data::SceneData, Data::ItemData, and
 * Data::CharacterData are its primary consumers today.
 */

#include <cstdint>

namespace ADS::Types {

    /**
     * @brief Phantom-typed wrapper around a uint8_t entity identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @tparam Tag Empty tag type distinguishing one entity kind's ids from
     *             another's at compile time (e.g. a SceneId cannot be
     *             implicitly compared with an ObjectId).
     */
    template<typename Tag>
    struct Id {
        uint8_t value; ///< The underlying identifier value

        /**
         * @brief Construct an Id from a raw uint8_t value
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param v The raw identifier value
         */
        explicit Id(uint8_t v) : value(v) {}

        /**
         * @brief Check equality with another Id of the same tag
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param other The Id to compare against
         * @return bool True if the underlying values are equal
         */
        bool operator==(const Id& other) const = default;
    };

    /// @brief Tag type distinguishing scene identifiers
    struct SceneTag {};

    /// @brief Tag type distinguishing object (item) identifiers
    struct ObjectTag {};

    /// @brief Tag type distinguishing event identifiers
    struct EventTag {};

    /// @brief Tag type distinguishing character identifiers
    struct CharacterTag {};

    /// @brief Tag type distinguishing state-catalog identifiers
    struct StateTag {};

    /// @brief Tag type distinguishing state-chain identifiers
    struct ChainTag {};

    using SceneId     = Id<SceneTag>;     ///< Unique identifier for a scene
    using ObjectId    = Id<ObjectTag>;    ///< Unique identifier for an item
    using EventId     = Id<EventTag>;     ///< Unique identifier for an event
    using CharacterId = Id<CharacterTag>; ///< Unique identifier for a character
    using StateId     = Id<StateTag>;     ///< Unique identifier for a state-catalog entry
    using ChainId     = Id<ChainTag>;     ///< Unique identifier for a named state chain

} // namespace ADS::Types

#endif // ADS_TYPES_ID_H