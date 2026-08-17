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

#ifndef ADS_LEXICON_TYPES_H
#define ADS_LEXICON_TYPES_H

/**
 * @file types.h
 * @brief Base types for the ADS Lexicon (Lexingine) compiler subsystem
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * NLP backend: pluggable via INLPBackend. udpipe::word is never referenced
 * here — the concrete backend adapter (e.g. a future UDPipeBackend) is
 * responsible for populating NLPToken. This keeps the Lexicon and this
 * header free of any third-party NLP dependency.
 *
 * @see ADS::Lexicon::INLPBackend
 * @see ADS::Lexicon::LexEntry
 */

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>
#include <boost/describe/members.hpp>

namespace ADS::Lexicon {

    // -------------------------------------------------------------------
    // LanguageCode — BCP-47 / POSIX locale with region: "es_ES", "en_US"
    //
    // Reuses the same string format as ADS::Constants::Languages and
    // ADS::i18n::LocaleInfo rather than introducing a parallel enum.
    // -------------------------------------------------------------------

    using LanguageCode = std::string;

    // -------------------------------------------------------------------
    // TokenIndex — compiled target token
    //
    //   0x0000              reserved: EOL (end of message)
    //   0x0001 - 0x00FE     1-byte token  (top-254 by frequency)
    //   0x00FF              reserved: escape byte
    //   0x0100 - 0xFFFE     3-byte token  [0xFF][hi][lo]
    //   0xFFFF              sentinel: UNASSIGNED
    // -------------------------------------------------------------------

    using TokenIndex = uint16_t;

    /**
     * @brief Constants and helpers for the TokenIndex encoding scheme
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    namespace Token {
        inline constexpr TokenIndex EOL             = 0x0000; ///< End-of-message marker
        inline constexpr TokenIndex ESCAPE          = 0x00FF; ///< Escape byte, never assigned as an id
        inline constexpr TokenIndex UNASSIGNED      = 0xFFFF; ///< Sentinel for an entry not yet indexed
        inline constexpr TokenIndex SINGLE_BYTE_MIN = 0x0001; ///< First 1-byte token id
        inline constexpr TokenIndex SINGLE_BYTE_MAX = 0x00FE; ///< Last 1-byte token id (254 slots)
        inline constexpr TokenIndex EXTENDED_MIN    = 0x0100; ///< First 3-byte token id
        inline constexpr TokenIndex EXTENDED_MAX    = 0xFFFE; ///< Last 3-byte token id

        /**
         * @brief Check whether a token id falls in the 1-byte range
         * @param t Token id to check
         * @return bool True if t is one of the top-254 most frequent tokens
         */
        [[nodiscard]] constexpr bool isSingleByte(TokenIndex t) noexcept {
            return t >= SINGLE_BYTE_MIN && t <= SINGLE_BYTE_MAX;
        }

        /**
         * @brief Check whether a token id falls in the 3-byte extended range
         * @param t Token id to check
         * @return bool True if t requires the [0xFF][hi][lo] encoding
         */
        [[nodiscard]] constexpr bool isExtended(TokenIndex t) noexcept {
            return t >= EXTENDED_MIN && t <= EXTENDED_MAX;
        }

        /**
         * @brief Number of bytes a token id occupies once encoded
         * @param t Token id to measure
         * @return uint8_t 1 for EOL/single-byte tokens, 3 for extended tokens, 0 if invalid
         */
        [[nodiscard]] constexpr uint8_t encodedSize(TokenIndex t) noexcept {
            if (t == EOL)          return 1;
            if (isSingleByte(t))   return 1;
            if (isExtended(t))     return 3;
            return 0;
        }
    } // namespace Token

    // -------------------------------------------------------------------
    // WordType — bitmask over uint16_t
    //
    // Each bit represents one grammatical category independently. A
    // single LexEntry may carry multiple bits simultaneously, reflecting
    // genuine lexical ambiguity observed in the corpus (e.g. Spanish
    // "abierto": Verb | Adjective).
    // -------------------------------------------------------------------

    using WordType = uint16_t;

    /**
     * @brief Constants and helpers for the WordType grammatical bitmask
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Bit layout (11 categories used, 5 spare for future extension):
     *   bit  0 - Verb          UD: VERB, AUX
     *   bit  1 - Noun          UD: NOUN, PROPN
     *   bit  2 - Adjective     UD: ADJ
     *   bit  3 - Adverb        UD: ADV
     *   bit  4 - Preposition   UD: ADP
     *   bit  5 - Article       UD: DET
     *   bit  6 - Conjunction   UD: CCONJ, SCONJ
     *   bit  7 - Pronoun       UD: PRON
     *   bit  8 - Interjection  UD: INTJ
     *   bit  9 - Punctuation   UD: PUNCT (filtered — never inserted)
     *   bit 10 - Other         UD: NUM, SYM, X, PART
     *   bits 11-15 - spare
     */
    namespace WordTypeBits {
        inline constexpr WordType None         = 0x0000;
        inline constexpr WordType Verb         = 1 << 0;
        inline constexpr WordType Noun         = 1 << 1;
        inline constexpr WordType Adjective    = 1 << 2;
        inline constexpr WordType Adverb       = 1 << 3;
        inline constexpr WordType Preposition  = 1 << 4;
        inline constexpr WordType Article      = 1 << 5;
        inline constexpr WordType Conjunction  = 1 << 6;
        inline constexpr WordType Pronoun      = 1 << 7;
        inline constexpr WordType Interjection = 1 << 8;
        inline constexpr WordType Punctuation  = 1 << 9;
        inline constexpr WordType Other        = 1 << 10;

        /// Number of defined bit positions (used to size per-entry occurrence arrays).
        inline constexpr std::size_t BIT_COUNT = 11;

        /// String representation indexed by bit position (0-10).
        inline constexpr std::array<std::string_view, BIT_COUNT> NAMES = {
            "verb", "noun", "adjective", "adverb", "preposition",
            "article", "conjunction", "pronoun", "interjection",
            "punctuation", "other",
        };

        /**
         * @brief Bit position of a single-bit WordType value
         * @param wordType A WordType value with exactly one bit set
         * @return std::size_t Zero-based bit position
         */
        [[nodiscard]] constexpr std::size_t bitPos(WordType wordType) noexcept {
            std::size_t pos = 0;
            while (wordType > 1) { wordType >>= 1; ++pos; }
            return pos;
        }

        /**
         * @brief Human-readable name for a single-bit WordType
         * @param wordType A WordType value with exactly one bit set
         * @return std::string_view Category name, or "unknown" if out of range
         */
        [[nodiscard]] constexpr std::string_view name(WordType wordType) noexcept {
            const auto pos = bitPos(wordType);
            return pos < BIT_COUNT ? NAMES[pos] : "unknown";
        }

        /**
         * @brief Check whether more than one category bit is set
         * @param wordType WordType bitmask to check
         * @return bool True if wordType carries two or more grammatical categories
         */
        [[nodiscard]] constexpr bool isAmbiguous(WordType wordType) noexcept {
            return wordType != None && (wordType & (wordType - 1)) != 0;
        }

        /**
         * @brief Check whether wordType has the given category bit set
         * @param wordType WordType bitmask to check
         * @param category Single-bit WordType to test for
         * @return bool True if category is present in wordType
         */
        [[nodiscard]] constexpr bool has(WordType wordType, WordType category) noexcept {
            return (wordType & category) != 0;
        }

        /**
         * @brief Universal Dependencies UPOS tag string constants
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fixed values from the external Universal Dependencies standard
         * (https://universaldependencies.org/u/pos/) — not project-defined,
         * kept here only to give fromUpos() and NLP backends a single named
         * source instead of duplicating the raw strings.
         */
        namespace UD {
            inline constexpr std::string_view VERB  = "VERB";
            inline constexpr std::string_view AUX   = "AUX";
            inline constexpr std::string_view NOUN  = "NOUN";
            inline constexpr std::string_view PROPN = "PROPN";
            inline constexpr std::string_view ADJ   = "ADJ";
            inline constexpr std::string_view ADV   = "ADV";
            inline constexpr std::string_view ADP   = "ADP";
            inline constexpr std::string_view DET   = "DET";
            inline constexpr std::string_view CCONJ = "CCONJ";
            inline constexpr std::string_view SCONJ = "SCONJ";
            inline constexpr std::string_view PRON  = "PRON";
            inline constexpr std::string_view INTJ  = "INTJ";
            inline constexpr std::string_view PUNCT = "PUNCT";
            inline constexpr std::string_view OTHER = "X"; ///< UD catch-all tag used for NUM/SYM/X/PART
        }

        /**
         * @brief Map a Universal Dependencies UPOS string to a single-bit WordType
         * @param upos Raw UPOS tag as returned by the NLP backend (e.g. "VERB")
         * @return WordType Single-bit category, defaulting to Other if unrecognised
         */
        [[nodiscard]] inline WordType fromUpos(std::string_view upos) noexcept {
            if (upos == UD::VERB || upos == UD::AUX)    return Verb;
            if (upos == UD::NOUN || upos == UD::PROPN)  return Noun;
            if (upos == UD::ADJ)                        return Adjective;
            if (upos == UD::ADV)                        return Adverb;
            if (upos == UD::ADP)                        return Preposition;
            if (upos == UD::DET)                        return Article;
            if (upos == UD::CCONJ || upos == UD::SCONJ) return Conjunction;
            if (upos == UD::PRON)                       return Pronoun;
            if (upos == UD::INTJ)                       return Interjection;
            if (upos == UD::PUNCT)                      return Punctuation;
            return Other;
        }
    } // namespace WordTypeBits

    // -------------------------------------------------------------------
    // EntryRole — participation in output text and/or parser input
    // -------------------------------------------------------------------

    enum class EntryRole : uint8_t {
        Output = 0b01, ///< Appears in text shown to the player, never typed
        Input  = 0b10, ///< Recognised by the parser, the player can type it
        Both   = 0b11, ///< Output and Input
    };

    /**
     * @brief Check whether a role includes the Output bit
     * @param r Role to check
     * @return bool True if the entry appears in text shown to the player
     */
    [[nodiscard]] constexpr bool hasOutput(EntryRole r) noexcept {
        return (static_cast<uint8_t>(r) & 0b01) != 0;
    }

    /**
     * @brief Check whether a role includes the Input bit
     * @param r Role to check
     * @return bool True if the entry is recognised by the parser
     */
    [[nodiscard]] constexpr bool hasInput(EntryRole r) noexcept {
        return (static_cast<uint8_t>(r) & 0b10) != 0;
    }

    // -------------------------------------------------------------------
    // LexEntryId — stable design-time handle
    // -------------------------------------------------------------------

    using LexEntryId = uint32_t;

    /// Reserved sentinel — never assigned to a real entry.
    inline constexpr LexEntryId INVALID_ENTRY_ID = std::numeric_limits<uint32_t>::max();

    // -------------------------------------------------------------------
    // NLPToken — plain data carrier produced by an INLPBackend
    // -------------------------------------------------------------------

    /**
     * @brief One analysed word, as produced by an NLP backend
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * A plain data carrier with no invariants: the backend fills it, the
     * Lexicon consumes it via Lexicon::record(), and it is then discarded.
     */
    struct NLPToken {
        std::string  form;                                  ///< Surface form as written
        std::string  lemma;                                 ///< Canonical dictionary form — Lexicon lookup key
        std::string  upos;                                  ///< Raw UD UPOS tag (e.g. "VERB")
        std::string  feats;                                 ///< UD morphological features string
        std::string  dependencyRelation;                    ///< Dependency relation (diagnostic only)
        int          head       = 0;                        ///< Dependency head index (diagnostic only)
        float        confidence = 1.0f;                     ///< 1.0 full model, 0.6 fallback tokenizer
        WordType     wordType   = WordTypeBits::None;       ///< Derived by the backend adapter
        LanguageCode lang;                                  ///< Language this token was analysed in

        /**
         * @brief Check whether this token should be inserted into the Lexicon
         * @return bool False for punctuation, unclassified, or empty-lemma tokens
         */
        [[nodiscard]] bool isLexical() const noexcept {
            return wordType != WordTypeBits::Punctuation && !lemma.empty();
        }
    };

    // -------------------------------------------------------------------
    // SynonymLink — directed synonym relationship
    //
    // Confidence by origin:
    //   1.0  manually confirmed by the author
    //   0.9  Layer 1 affix match + root exists in the Lexicon
    //   0.7  Layer 1 affix match only, root not yet in the Lexicon
    //   0.6  Layer 2 shared stem (fallback mode, no full NLP model)
    //   0.4  Layer 3 semantic-similarity candidate, pending review
    // -------------------------------------------------------------------

    /**
     * @brief One directed synonym relationship between two LexEntry ids
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    struct SynonymLink {
        LexEntryId target     = INVALID_ENTRY_ID; ///< Id of the equivalent entry
        float      confidence = 0.0f;             ///< Confidence in [0.0, 1.0], see origin table above
        bool       confirmed  = false;            ///< True only when set by the author
    };

} // namespace ADS::Lexicon

#endif // ADS_LEXICON_TYPES_H