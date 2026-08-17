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

namespace ADS::Lexicon {

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

    void LexEntry::accumulate(uint32_t totalTokens) noexcept {
        ++m_rawCount;
        m_frequency = totalTokens > 0
            ? static_cast<float>(m_rawCount) / static_cast<float>(totalTokens)
            : 0.0f;
    }

    const SynonymLink* LexEntry::findSynonym(LexEntryId targetId) const noexcept {
        for (const auto& s : synonyms) {
            if (s.target == targetId) return &s;
        }
        return nullptr;
    }

    std::partial_ordering LexEntry::operator<=>(const LexEntry& other) const noexcept {
        if (m_frequency != other.m_frequency) return other.m_frequency <=> m_frequency;
        return canonical <=> other.canonical;
    }

} // namespace ADS::Lexicon