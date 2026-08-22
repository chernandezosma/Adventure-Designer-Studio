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
 * @file FallbackNLPBackend.cpp
 * @brief Implementation of FallbackNLPBackend
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "FallbackNLPBackend.h"

#include <array>
#include <cctype>
#include <unordered_map>

namespace ADS::LexEngine {

    namespace {

        /// UTF-8 two-byte accented Latin sequence -> plain ASCII fold.
        ///
        /// Covers both lower- and upper-case forms: std::tolower() only
        /// affects single ASCII bytes, so an upper-case accented letter
        /// (e.g. "\xC3\x93" = 'Ó') passes through the byte-wise lowering
        /// step untouched and must be folded here directly.
        const std::unordered_map<std::string, char> kDiacriticFold = {
            {"\xC3\xA1", 'a'}, {"\xC3\xA0", 'a'}, {"\xC3\xA2", 'a'}, {"\xC3\xA4", 'a'}, {"\xC3\xA3", 'a'},
            {"\xC3\x81", 'a'}, {"\xC3\x80", 'a'}, {"\xC3\x82", 'a'}, {"\xC3\x84", 'a'}, {"\xC3\x83", 'a'},
            {"\xC3\xA9", 'e'}, {"\xC3\xA8", 'e'}, {"\xC3\xAA", 'e'}, {"\xC3\xAB", 'e'},
            {"\xC3\x89", 'e'}, {"\xC3\x88", 'e'}, {"\xC3\x8A", 'e'}, {"\xC3\x8B", 'e'},
            {"\xC3\xAD", 'i'}, {"\xC3\xAC", 'i'}, {"\xC3\xAE", 'i'}, {"\xC3\xAF", 'i'},
            {"\xC3\x8D", 'i'}, {"\xC3\x8C", 'i'}, {"\xC3\x8E", 'i'}, {"\xC3\x8F", 'i'},
            {"\xC3\xB3", 'o'}, {"\xC3\xB2", 'o'}, {"\xC3\xB4", 'o'}, {"\xC3\xB6", 'o'}, {"\xC3\xB5", 'o'},
            {"\xC3\x93", 'o'}, {"\xC3\x92", 'o'}, {"\xC3\x94", 'o'}, {"\xC3\x96", 'o'}, {"\xC3\x95", 'o'},
            {"\xC3\xBA", 'u'}, {"\xC3\xB9", 'u'}, {"\xC3\xBB", 'u'}, {"\xC3\xBC", 'u'},
            {"\xC3\x9A", 'u'}, {"\xC3\x99", 'u'}, {"\xC3\x9B", 'u'}, {"\xC3\x9C", 'u'},
            {"\xC3\xB1", 'n'}, {"\xC3\x91", 'n'},
            {"\xC3\xA7", 'c'}, {"\xC3\x87", 'c'},
        };

        /// Base language subtag from a LanguageCode, e.g. "es_ES" -> "es".
        [[nodiscard]] std::string baseLang(const LanguageCode& lang) noexcept {
            if (const auto pos = lang.find('_'); pos != std::string::npos) return lang.substr(0, pos);
            return lang;
        }

        /// True for ASCII alphanumerics and any UTF-8 continuation/lead byte, so multi-byte accented letters tokenise as one word.
        [[nodiscard]] bool isWordByte(unsigned char c) noexcept {
            return std::isalnum(c) || c >= 0x80;
        }

        /// Per-language suffix table, longest suffix first so it matches before shorter ones.
        [[nodiscard]] const std::vector<std::string_view>& suffixesFor(const std::string& base) {
            static const std::vector<std::string_view> es = {
                "aciones", "amiento", "imiento", "amente", "adores", "adora",
                "ando", "iendo", "ador", "able", "ible", "oso", "osa",
                "as", "es", "os", "a", "e", "o",
            };
            static const std::vector<std::string_view> en = {
                "ational", "ization", "iveness", "fulness", "ousness",
                "ing", "edly", "ment", "ness", "tion", "ous", "ive",
                "ed", "es", "ly", "s",
            };
            static const std::vector<std::string_view> de = {
                "ungen", "heiten", "keiten", "lich", "isch", "ung", "heit",
                "keit", "en", "er", "es", "e", "n",
            };
            static const std::vector<std::string_view> fr = {
                "issement", "ations", "ement", "ation", "eurs", "euse",
                "ants", "ant", "és", "er", "ir", "s", "e",
            };
            static const std::vector<std::string_view> pt = {
                "acoes", "amento", "imento", "amente", "adores", "adora",
                "ando", "endo", "ador", "avel", "ivel", "oso", "osa",
                "as", "es", "os", "a", "e", "o",
            };
            static const std::vector<std::string_view> none = {};

            if (base == "es") return es;
            if (base == "en") return en;
            if (base == "de") return de;
            if (base == "fr") return fr;
            if (base == "pt") return pt;
            return none;
        }

    } // namespace

    std::string FallbackNLPBackend::normalise(std::string_view word) {
        std::string lowered;
        lowered.reserve(word.size());
        for (const char c : word) {
            lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }

        std::string folded;
        folded.reserve(lowered.size());
        for (std::size_t i = 0; i < lowered.size();) {
            if (i + 1 < lowered.size()) {
                if (const auto it = kDiacriticFold.find(lowered.substr(i, 2)); it != kDiacriticFold.end()) {
                    folded.push_back(it->second);
                    i += 2;
                    continue;
                }
            }
            folded.push_back(lowered[i]);
            ++i;
        }
        return folded;
    }

    std::string FallbackNLPBackend::stem(std::string_view normalisedWord, const LanguageCode& lang) {
        const std::string base = baseLang(lang);
        const auto& suffixes = suffixesFor(base);

        // Keep at least a 3-character root so short words are never emptied out.
        constexpr std::size_t kMinRootLength = 3;

        for (const auto& suffix : suffixes) {
            if (normalisedWord.size() > suffix.size() + kMinRootLength &&
                normalisedWord.ends_with(suffix)) {
                return std::string(normalisedWord.substr(0, normalisedWord.size() - suffix.size()));
            }
        }
        return std::string(normalisedWord);
    }

    std::vector<NLPToken> FallbackNLPBackend::analyse(std::string_view sentence, const LanguageCode& lang) {
        std::vector<NLPToken> tokens;

        std::size_t i = 0;
        while (i < sentence.size()) {
            const auto c = static_cast<unsigned char>(sentence[i]);

            if (std::isspace(c)) {
                ++i;
                continue;
            }

            if (isWordByte(c)) {
                const std::size_t start = i;
                while (i < sentence.size() && isWordByte(static_cast<unsigned char>(sentence[i]))) ++i;
                const std::string_view form = sentence.substr(start, i - start);

                const std::string normalised = normalise(form);

                NLPToken token;
                token.form       = std::string(form);
                token.lemma      = stem(normalised, lang); // fallback: lemma sourced from the stemmer
                token.upos       = std::string(WordTypeBits::UD::OTHER);
                token.confidence = 0.6f;
                token.wordType   = WordTypeBits::Other;
                token.lang       = lang;
                tokens.push_back(std::move(token));
                continue;
            }

            // Any remaining non-space, non-word byte is treated as punctuation.
            NLPToken token;
            token.form       = std::string(1, sentence[i]);
            token.lemma       = token.form;
            token.upos        = std::string(WordTypeBits::UD::PUNCT);
            token.confidence  = 0.6f;
            token.wordType    = WordTypeBits::Punctuation;
            token.lang        = lang;
            tokens.push_back(std::move(token));
            ++i;
        }

        return tokens;
    }

} // namespace ADS::LexEngine