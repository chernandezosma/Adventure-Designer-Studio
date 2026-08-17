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

#ifndef ADS_LEXICON_ILEXICON_LOOKUP_H
#define ADS_LEXICON_ILEXICON_LOOKUP_H

/**
 * @file ILexiconLookup.h
 * @brief Minimal read-only Lexicon view consumed by SynonymPipeline
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * @see ADS::Lexicon::SynonymPipeline
 * @see ADS::Lexicon::Lexicon
 */

#include <string_view>
#include <vector>

#include "types.h"

namespace ADS::Lexicon {

    /**
     * @brief Minimal read-only view of the Lexicon needed by the synonym pipeline
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Implemented by Lexicon. Kept as a narrow interface so SynonymPipeline
     * does not depend on the full Lexicon container type.
     */
    class ILexiconLookup {
    public:
        virtual ~ILexiconLookup() = default;

        /**
         * @brief Resolve a canonical or synonym form to its owning entry id
         * @param form Word form to resolve
         * @param lang Language to search within
         * @return LexEntryId Owning entry id, or INVALID_ENTRY_ID if not found
         */
        [[nodiscard]] virtual LexEntryId findEntryId(std::string_view form, const LanguageCode& lang) const = 0;

        /**
         * @brief Collect the ids of every entry in a language sharing a given stem
         * @param stemValue Stem value to match against LexEntry::stem
         * @param lang Language to search within
         * @param excluding Entry id to omit from the results (the entry being processed)
         * @return std::vector<LexEntryId> Matching entry ids, possibly empty
         */
        [[nodiscard]] virtual std::vector<LexEntryId> findByStem(std::string_view stemValue,
                                                                   const LanguageCode& lang,
                                                                   LexEntryId excluding) const = 0;
    };

} // namespace ADS::Lexicon

#endif // ADS_LEXICON_ILEXICON_LOOKUP_H
