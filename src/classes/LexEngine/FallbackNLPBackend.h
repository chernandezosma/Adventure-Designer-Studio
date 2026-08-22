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

#ifndef ADS_LEXENGINE_FALLBACK_NLP_BACKEND_H
#define ADS_LEXENGINE_FALLBACK_NLP_BACKEND_H

/**
 * @file FallbackNLPBackend.h
 * @brief Rule-based NLP backend used when no full NLP model is available
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>
#include <string_view>
#include <vector>

#include "INLPBackend.h"
#include "types.h"

namespace ADS::LexEngine {

    /**
     * @brief Rule-based tokenizer and suffix-stripping stemmer fallback backend
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Used when no full NLP model is available for a language. Produces
     * NLPToken::confidence = 0.6 and leaves NLPToken::wordType as
     * WordTypeBits::Other — the author classifies the entry manually via
     * the IDE. NLPToken::lemma is populated from a lightweight
     * suffix-stripping stemmer rather than true lemmatisation, matching
     * the documented "lemma source: Snowball stemmer (approximate)"
     * fallback behaviour.
     */
    class FallbackNLPBackend final : public INLPBackend {
    public:
        /**
         * @brief Analyse a sentence using whitespace/punctuation tokenisation and stemming
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sentence Full sentence text to analyse
         * @param lang Language the sentence is written in — selects the stemming ruleset
         * @return std::vector<NLPToken> One token per word or punctuation mark, in order
         */
        [[nodiscard]] std::vector<NLPToken> analyse(std::string_view sentence,
                                                      const LanguageCode& lang) override;

        /**
         * @brief Lowercase a word and strip common Latin diacritics
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param word Surface form to normalise
         * @return std::string Lowercased, diacritic-stripped form
         */
        [[nodiscard]] static std::string normalise(std::string_view word);

        /**
         * @brief Apply a lightweight suffix-stripping stem for the given language
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Not a full Snowball implementation — a small per-language suffix
         * table sufficient to group common inflected forms until a real
         * NLP backend is wired in.
         *
         * @param normalisedWord Already-normalised (lowercase, no diacritics) word
         * @param lang Language selecting the suffix ruleset (es/en/de/fr/pt); other codes pass through unchanged
         * @return std::string Stemmed root
         */
        [[nodiscard]] static std::string stem(std::string_view normalisedWord, const LanguageCode& lang);
    };

} // namespace ADS::LexEngine

#endif // ADS_LEXENGINE_FALLBACK_NLP_BACKEND_H