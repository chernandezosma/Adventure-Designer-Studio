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

#ifndef ADS_LEXICON_LEXICON_H
#define ADS_LEXICON_LEXICON_H

/**
 * @file Lexicon.h
 * @brief The Lexicon container — accumulates and indexes a game's vocabulary
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Holds one ordered collection per active language, sorted by descending
 * frequency once index() has run, plus a secondary lookup index resolving
 * a canonical form to its entry in O(1). Built incrementally as the author
 * writes text in the IDE via feed()/record().
 *
 * @see ADS::Lexicon::LexEntry
 * @see ADS::Lexicon::INLPBackend
 * @see ADS::Lexicon::SynonymPipeline
 */

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "INLPBackend.h"
#include "LexEntry.h"
#include "SynonymPipeline.h"
#include "types.h"

namespace ADS::Lexicon {

    /**
     * @brief Accumulates, indexes, and looks up a game's per-language vocabulary
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Non-copyable: owns every LexEntry via std::unique_ptr. Implements
     * ILexiconLookup so SynonymPipeline can validate candidate roots and
     * find stem siblings without depending on the full Lexicon type.
     */
    class Lexicon final : public ILexiconLookup {
    public:
        Lexicon() = default;
        ~Lexicon() override = default;

        Lexicon(const Lexicon&) = delete;
        Lexicon& operator=(const Lexicon&) = delete;

        /**
         * @brief Analyse a sentence and record every lexical token it contains
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * The incremental entry point the IDE calls as the author types.
         * Punctuation and empty tokens returned by the backend are skipped.
         *
         * @param sentence Full sentence text to analyse
         * @param lang Language the sentence is written in
         * @param backend NLP backend used to tokenise and classify the sentence
         */
        void feed(std::string_view sentence, const LanguageCode& lang, INLPBackend& backend);

        /**
         * @brief Look up an existing entry by lemma+language, or insert a new one
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * On a hit, updates the existing entry's type observation and
         * frequency. On a miss, creates a new entry, observes its type,
         * accumulates its frequency, and runs the synonym pipeline against
         * it. Tokens that are not lexical (NLPToken::isLexical() == false)
         * are ignored and nullptr is returned.
         *
         * @param token Analysed word to record
         * @return LexEntry* Non-owning pointer to the recorded entry, or nullptr if the token was filtered
         */
        LexEntry* record(const NLPToken& token);

        /**
         * @brief Assign TokenIndex values to every entry, per language
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Sorts each language's entries by descending frequency (alphabetical
         * tiebreak), assigns the top 254 entries a 1-byte TokenIndex
         * (0x0001-0x00FE) and all remaining entries a 3-byte TokenIndex
         * (0x0100-0xFFFE). Logs a warning for every ambiguous entry
         * (LexEntry::isAmbiguous() == true).
         */
        void index();

        /**
         * @brief Get the entry collection for one language, ordered by index() if it has run
         * @param lang Language to query
         * @return const std::vector<std::unique_ptr<LexEntry>>& Entries for that language, empty if none recorded
         */
        [[nodiscard]] const std::vector<std::unique_ptr<LexEntry>>& getEntries(const LanguageCode& lang) const;

        /**
         * @brief Find an entry by its stable design-time id
         * @param id Entry id to look up
         * @return LexEntry* Non-owning pointer, or nullptr if not found
         */
        [[nodiscard]] LexEntry* findById(LexEntryId id) const;

        // -- ILexiconLookup ------------------------------------------------

        /**
         * @brief Resolve a canonical form to its owning entry id
         * @param form Canonical form to resolve
         * @param lang Language to search within
         * @return LexEntryId Owning entry id, or INVALID_ENTRY_ID if not found
         */
        [[nodiscard]] LexEntryId findEntryId(std::string_view form, const LanguageCode& lang) const override;

        /**
         * @brief Collect the ids of every entry in a language sharing a given stem
         * @param stemValue Stem value to match against LexEntry::stem
         * @param lang Language to search within
         * @param excluding Entry id to omit from the results
         * @return std::vector<LexEntryId> Matching entry ids, possibly empty
         */
        [[nodiscard]] std::vector<LexEntryId> findByStem(std::string_view stemValue,
                                                           const LanguageCode& lang,
                                                           LexEntryId excluding) const override;

    private:
        /// Owned entries, partitioned by language, ordered by index() once it has run.
        std::unordered_map<LanguageCode, std::vector<std::unique_ptr<LexEntry>>> m_entriesByLang;

        /// O(1) lookup: "<lang>:<canonical>" -> entry id.
        std::unordered_map<std::string, LexEntryId> m_lookupIndex;

        /// O(1) lookup: entry id -> non-owning raw pointer, valid for the entry's lifetime.
        std::unordered_map<LexEntryId, LexEntry*> m_byId;

        /// Total observed token count per language, used to recompute frequency.
        std::unordered_map<LanguageCode, uint32_t> m_totalTokens;

        LexEntryId m_nextId = 0; ///< Next id to assign — monotonically increasing, never reused
    };

} // namespace ADS::Lexicon

#endif // ADS_LEXICON_LEXICON_H