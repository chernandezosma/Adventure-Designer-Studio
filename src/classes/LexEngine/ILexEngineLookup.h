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

#ifndef ADS_LEXENGINE_ILEXENGINE_LOOKUP_H
#define ADS_LEXENGINE_ILEXENGINE_LOOKUP_H

/**
 * @file ILexEngineLookup.h
 * @brief Minimal read-only LexEngine view consumed by SynonymPipeline
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * @see ADS::LexEngine::SynonymPipeline
 * @see ADS::LexEngine::LexEngine
 */

#include <string_view>
#include <vector>

#include "types.h"

namespace ADS::LexEngine {

    /**
     * @brief Minimal read-only view of the LexEngine needed by the synonym pipeline
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Implemented by LexEngine. Kept as a narrow interface so SynonymPipeline
     * does not depend on the full LexEngine container type.
     */
    class ILexEngineLookup {
    public:
        virtual ~ILexEngineLookup() = default;

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

} // namespace ADS::LexEngine

#endif // ADS_LEXENGINE_ILEXENGINE_LOOKUP_H
