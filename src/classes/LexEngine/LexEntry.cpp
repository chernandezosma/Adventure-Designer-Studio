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
 * @file LexEntry.cpp
 * @brief Implementation of LexEntry
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "LexEntry.h"

#include <algorithm>
#include <cassert>

namespace ADS::LexEngine {

    /**
     * @brief Record one new occurrence of a given single-bit WordType
     *
     * Updates the bitmask, the per-bit occurrence count, and recalculates
     * the dominant type. Precondition: singleBit must have exactly one
     * bit set and must not be WordTypeBits::None.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param singleBit Single-bit WordType observed for this occurrence
     */
    void LexEntry::observeType(WordType singleBit) noexcept {
        assert(singleBit != WordTypeBits::None);
        assert((singleBit & (singleBit - 1)) == 0); // must be single-bit

        const auto pos = WordTypeBits::bitPos(singleBit);
        assert(pos < WordTypeBits::BIT_COUNT);

        m_types |= singleBit;
        ++m_typeCounts[pos];

        const auto maxPos = static_cast<std::size_t>(
            std::max_element(m_typeCounts.begin(), m_typeCounts.end()) - m_typeCounts.begin());
        m_dominantType = static_cast<WordType>(1 << maxPos);
    }

    /**
     * @brief Record one new occurrence and recompute frequency
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param totalTokens Current total corpus token count, including this occurrence
     */
    void LexEntry::accumulate(uint32_t totalTokens) noexcept {
        ++m_rawCount;
        m_frequency = totalTokens > 0
            ? static_cast<float>(m_rawCount) / static_cast<float>(totalTokens)
            : 0.0f;
    }

    /**
     * @brief Find a synonym link to a given target entry, if one exists
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param targetId Id of the entry to search for
     * @return const SynonymLink* Pointer to the link, or nullptr if not found
     */
    const SynonymLink* LexEntry::findSynonym(LexEntryId targetId) const noexcept {
        for (const auto& s : synonyms) {
            if (s.target == targetId) return &s;
        }
        return nullptr;
    }

    /**
     * @brief Order entries by descending frequency, alphabetical tiebreak
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param other Entry to compare against
     * @return std::partial_ordering Ordering result used for index-time sorting
     */
    std::partial_ordering LexEntry::operator<=>(const LexEntry& other) const noexcept {
        if (m_frequency != other.m_frequency) return other.m_frequency <=> m_frequency;
        return canonical <=> other.canonical;
    }

} // namespace ADS::LexEngine