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

#ifndef ADS_LEXICON_INLP_BACKEND_H
#define ADS_LEXICON_INLP_BACKEND_H

/**
 * @file INLPBackend.h
 * @brief NLP backend abstraction consumed by the Lexicon
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * The Lexicon never depends on a concrete NLP library directly. A future
 * UDPipe-backed implementation can be added behind this interface once
 * packaging (vcpkg overlay port or FetchContent) is resolved; today only
 * FallbackNLPBackend is provided.
 *
 * @see ADS::Lexicon::FallbackNLPBackend
 */

#include <string_view>
#include <vector>

#include "types.h"

namespace ADS::Lexicon {

    /**
     * @brief Abstract interface for a sentence-level NLP analysis backend
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Implementations analyse a complete sentence — not isolated words —
     * since correct disambiguation of lexically ambiguous forms requires
     * full-sentence context.
     */
    class INLPBackend {
    public:
        virtual ~INLPBackend() = default;

        /**
         * @brief Analyse a complete sentence and return one NLPToken per word
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sentence Full sentence text to analyse
         * @param lang Language the sentence is written in
         * @return std::vector<NLPToken> One token per recognised word, in order
         */
        [[nodiscard]] virtual std::vector<NLPToken> analyse(std::string_view sentence,
                                                              const LanguageCode& lang) = 0;
    };

} // namespace ADS::Lexicon

#endif // ADS_LEXICON_INLP_BACKEND_H