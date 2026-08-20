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

#ifndef ADS_LEXENGINE_LEX_ENTRY_H
#define ADS_LEXENGINE_LEX_ENTRY_H

/**
 * @file LexEntry.h
 * @brief Atomic vocabulary unit of the LexEngine compiler subsystem
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <array>
#include <compare>
#include <cstdint>
#include <string>
#include <vector>

#include "types.h"

namespace ADS::LexEngine {

    /**
     * @brief One canonical word form within a specific language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * A class (not a plain struct) because the `types` bitmask and the
     * per-bit occurrence counts must stay in sync — observeType() is the
     * single controlled mutation point for both. Likewise raw_count and
     * frequency are only ever changed together, through accumulate().
     *
     * Lifecycle:
     *   1. Created when the NLP backend returns a lemma not yet in the LexEngine.
     *   2. On each subsequent occurrence, observeType() updates the bitmask,
     *      the per-bit counts, and recalculates the dominant type.
     *   3. The author may add synonyms and adjust the role via the IDE.
     *   4. setCompiledToken() is called by LexEngine::index() at compile time.
     */
    class LexEntry {
    public:
        // -- Identity (set at construction, immutable) ------------------

        LexEntryId   id   = INVALID_ENTRY_ID; ///< Stable design-time identifier
        LanguageCode lang;                    ///< BCP-47 / POSIX locale, e.g. "es_ES"
        std::string  canonical;               ///< Normalised word form, max 32 chars
        EntryRole    role = EntryRole::Both;   ///< Output, Input, or Both

        // -- Linguistic metadata (public read, controlled write) --------

        std::string upos_raw;                 ///< Raw UPOS string from the NLP backend — diagnostic only
        std::string feats;                    ///< UD morphological features — used by the synonym pipeline
        std::string stem;                     ///< Snowball-style stem, populated only in fallback mode

        std::vector<SynonymLink> synonyms;    ///< Directed synonym relationships

        /**
         * @brief Combined bitmask of every WordType observed for this entry
         * @return WordType All grammatical categories seen in the corpus so far
         */
        [[nodiscard]] WordType types() const noexcept { return m_types; }

        /**
         * @brief Single WordType bit with the highest observed occurrence count
         * @return WordType The dominant grammatical category, recalculated on every observation
         */
        [[nodiscard]] WordType dominantType() const noexcept { return m_dominantType; }

        /**
         * @brief Occurrence count for one specific single-bit WordType
         * @param singleBit Single-bit WordType to query
         * @return uint32_t Number of times singleBit has been observed, 0 if out of range
         */
        [[nodiscard]] uint32_t typeCount(WordType singleBit) const noexcept {
            const auto pos = WordTypeBits::bitPos(singleBit);
            return pos < WordTypeBits::BIT_COUNT ? m_typeCounts[pos] : 0;
        }

        /**
         * @brief Check whether this entry carries more than one grammatical category
         * @return bool True if the entry is lexically ambiguous
         */
        [[nodiscard]] bool isAmbiguous() const noexcept {
            return WordTypeBits::isAmbiguous(m_types);
        }

        /**
         * @brief Record one new occurrence of a given single-bit WordType
         *
         * Updates the bitmask, the per-bit occurrence count, and recalculates
         * the dominant type. Precondition: singleBit must have exactly one
         * bit set and must not be WordTypeBits::None.
         *
         * @param singleBit Single-bit WordType observed for this occurrence
         */
        void observeType(WordType singleBit) noexcept;

        /**
         * @brief Get the normalised occurrence frequency
         * @return float raw_count / total_corpus_tokens at the time of the last accumulate() call
         */
        [[nodiscard]] float frequency() const noexcept { return m_frequency; }

        /**
         * @brief Get the absolute occurrence count
         * @return uint32_t Monotonically increasing raw occurrence count
         */
        [[nodiscard]] uint32_t rawCount() const noexcept { return m_rawCount; }

        /**
         * @brief Record one new occurrence and recompute frequency
         * @param totalTokens Current total corpus token count, including this occurrence
         */
        void accumulate(uint32_t totalTokens) noexcept;

        /**
         * @brief Restore the combined type bitmask from persisted JSON
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Used only by LexEngineSerializer when loading. Per-bit occurrence
         * counts and the dominant type are NOT restored — the persistence
         * contract documented in vocabulary.md keeps type_counts and
         * dominant_type as recalculated-on-load fields with no companion
         * history file in this pass, so dominantType() stays
         * WordTypeBits::None until new observations arrive via observeType().
         *
         * @param types Combined WordType bitmask to restore
         */
        void restoreTypes(WordType types) noexcept { m_types = types; }

        /**
         * @brief Restore raw_count and frequency from persisted JSON
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Used only by LexEngineSerializer when loading, bypassing
         * accumulate()'s increment semantics since these are exact
         * persisted values rather than a new observation.
         *
         * @param rawCount Absolute occurrence count to restore
         * @param frequency Normalised occurrence frequency to restore
         */
        void restoreCounts(uint32_t rawCount, float frequency) noexcept {
            m_rawCount  = rawCount;
            m_frequency = frequency;
        }

        /**
         * @brief Get the compiled token index assigned at index time
         * @return TokenIndex Token::UNASSIGNED until LexEngine::index() has run
         */
        [[nodiscard]] TokenIndex compiledToken() const noexcept { return m_compiledToken; }

        /**
         * @brief Check whether this entry has been assigned a compiled token
         * @return bool True once LexEngine::index() has processed this entry
         */
        [[nodiscard]] bool isIndexed() const noexcept {
            return m_compiledToken != Token::UNASSIGNED;
        }

        /**
         * @brief Get the encoded byte size of the compiled token
         * @return uint8_t 1 for a single-byte token, 3 for extended, 0 if unassigned
         */
        [[nodiscard]] uint8_t encodedSize() const noexcept {
            return Token::encodedSize(m_compiledToken);
        }

        /**
         * @brief Assign the compiled token index — called by LexEngine::index() only
         * @param t Token index to assign to this entry
         */
        void setCompiledToken(TokenIndex t) noexcept { m_compiledToken = t; }

        /**
         * @brief Find a synonym link to a given target entry, if one exists
         * @param targetId Id of the entry to search for
         * @return const SynonymLink* Pointer to the link, or nullptr if not found
         */
        [[nodiscard]] const SynonymLink* findSynonym(LexEntryId targetId) const noexcept;

        /**
         * @brief Order entries by descending frequency, alphabetical tiebreak
         * @param other Entry to compare against
         * @return std::partial_ordering Ordering result used for index-time sorting
         */
        [[nodiscard]] std::partial_ordering operator<=>(const LexEntry& other) const noexcept;

        /**
         * @brief Identity comparison by entry id
         * @param other Entry to compare against
         * @return bool True if both entries share the same id
         */
        [[nodiscard]] bool operator==(const LexEntry& other) const noexcept {
            return id == other.id;
        }

    private:
        WordType m_types        = WordTypeBits::None; ///< Combined bitmask of observed categories
        WordType m_dominantType = WordTypeBits::None; ///< Category with the highest occurrence count

        /// Occurrence count per bit position, parallel to the bitmask.
        std::array<uint32_t, WordTypeBits::BIT_COUNT> m_typeCounts{};

        float      m_frequency     = 0.0f;             ///< raw_count / total_corpus_tokens
        uint32_t   m_rawCount      = 0;                ///< Absolute occurrence count, never decremented
        TokenIndex m_compiledToken = Token::UNASSIGNED; ///< Assigned by LexEngine::index() at compile time
    };

} // namespace ADS::LexEngine

#endif // ADS_LEXENGINE_LEX_ENTRY_H