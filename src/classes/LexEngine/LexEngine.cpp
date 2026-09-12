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
 * @file LexEngine.cpp
 * @brief Implementation of the LexEngine container
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "LexEngine.h"

#include <algorithm>

#include "spdlog/spdlog.h"

namespace ADS::LexEngine {

    namespace {
        /// Maximum length of LexEntry::canonical, per the documented field constraint.
        constexpr std::size_t kMaxCanonicalLength = 32;

        /// Builds the "<lang>:<form>" composite key used by m_lookupIndex.
        [[nodiscard]] std::string lookupKey(std::string_view form, const LanguageCode& lang) {
            std::string key;
            key.reserve(lang.size() + 1 + form.size());
            key.append(lang).append(":").append(form);
            return key;
        }
    } // namespace

    /**
     * @brief Analyse a sentence and record every lexical token it contains
     *
     * The incremental entry point the IDE calls as the author types.
     * Punctuation and empty tokens returned by the backend are skipped.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param sentence Full sentence text to analyse
     * @param lang Language the sentence is written in
     * @param backend NLP backend used to tokenise and classify the sentence
     */
    void LexEngine::feed(std::string_view sentence, const LanguageCode& lang, INLPBackend& backend) {
        for (const NLPToken& token : backend.analyse(sentence, lang)) {
            record(token);
        }
    }

    /**
     * @brief Look up an existing entry by lemma+language, or insert a new one
     *
     * On a hit, updates the existing entry's type observation and
     * frequency. On a miss, creates a new entry, observes its type,
     * accumulates its frequency, and runs the synonym pipeline against
     * it. Tokens that are not lexical (NLPToken::isLexical() == false)
     * are ignored and nullptr is returned.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param token Analysed word to record
     * @return LexEntry* Non-owning pointer to the recorded entry, or nullptr if the token was filtered
     */
    LexEntry* LexEngine::record(const NLPToken& token) {
        if (!token.isLexical()) {
            return nullptr;
        }

        const std::string key = lookupKey(token.lemma, token.lang);

        if (const auto it = m_lookupIndex.find(key); it != m_lookupIndex.end()) {
            LexEntry* entry = m_byId.at(it->second);
            entry->observeType(token.wordType);
            entry->accumulate(++m_totalTokens[token.lang]);
            return entry;
        }

        auto owned = std::make_unique<LexEntry>();
        LexEntry* entry = owned.get();

        entry->id        = m_nextId++;
        entry->lang       = token.lang;
        entry->canonical  = token.lemma.substr(0, kMaxCanonicalLength);
        entry->upos_raw   = token.upos;
        entry->feats      = token.feats;
        if (token.confidence < 1.0f) {
            entry->stem = token.lemma; // fallback mode: lemma is already stemmer-derived
        }

        entry->observeType(token.wordType);
        entry->accumulate(++m_totalTokens[token.lang]);

        m_byId.emplace(entry->id, entry);
        m_lookupIndex.emplace(key, entry->id);
        m_entriesByLang[token.lang].push_back(std::move(owned));

        entry->synonyms = SynonymPipeline::run(entry->canonical, entry->id, entry->lang, *this);

        return entry;
    }

    /**
     * @brief Assign TokenIndex values to every entry, per language
     *
     * Sorts each language's entries by descending frequency (alphabetical
     * tiebreak), assigns the top 254 entries a 1-byte TokenIndex
     * (0x0001-0x00FE) and all remaining entries a 3-byte TokenIndex
     * (0x0100-0xFFFE). Logs a warning for every ambiguous entry
     * (LexEntry::isAmbiguous() == true).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    void LexEngine::index() {
        for (auto& [lang, entries] : m_entriesByLang) {
            std::sort(entries.begin(), entries.end(),
                [](const std::unique_ptr<LexEntry>& a, const std::unique_ptr<LexEntry>& b) {
                    return *a < *b;
                });

            for (std::size_t rank = 0; rank < entries.size(); ++rank) {
                LexEntry& entry = *entries[rank];

                if (rank < static_cast<std::size_t>(Token::SINGLE_BYTE_MAX - Token::SINGLE_BYTE_MIN + 1)) {
                    entry.setCompiledToken(static_cast<TokenIndex>(Token::SINGLE_BYTE_MIN + rank));
                } else {
                    const std::size_t extendedRank = rank - (Token::SINGLE_BYTE_MAX - Token::SINGLE_BYTE_MIN + 1);
                    const auto tokenValue = static_cast<TokenIndex>(Token::EXTENDED_MIN + extendedRank);
                    if (tokenValue > Token::EXTENDED_MAX) {
                        spdlog::warn("LexEngine: vocabulary for '{}' exceeds addressable TokenIndex range "
                                     "({} entries) — '{}' left unassigned", lang, entries.size(), entry.canonical);
                        continue;
                    }
                    entry.setCompiledToken(tokenValue);
                }

                if (entry.isAmbiguous()) {
                    spdlog::warn("LexEngine: ambiguous entry '{}' ({}) — dominant type '{}'",
                                 entry.canonical, lang, WordTypeBits::name(entry.dominantType()));
                }
            }
        }
    }

    /**
     * @brief Get the entry collection for one language, ordered by index() if it has run
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param lang Language to query
     * @return const std::vector<std::unique_ptr<LexEntry>>& Entries for that language, empty if none recorded
     */
    const std::vector<std::unique_ptr<LexEntry>>& LexEngine::getEntries(const LanguageCode& lang) const {
        static const std::vector<std::unique_ptr<LexEntry>> kEmpty;
        const auto it = m_entriesByLang.find(lang);
        return it != m_entriesByLang.end() ? it->second : kEmpty;
    }

    /**
     * @brief Find an entry by its stable design-time id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param id Entry id to look up
     * @return LexEntry* Non-owning pointer, or nullptr if not found
     */
    LexEntry* LexEngine::findById(LexEntryId id) const {
        const auto it = m_byId.find(id);
        return it != m_byId.end() ? it->second : nullptr;
    }

    /**
     * @brief List every language that currently has at least one entry
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::vector<LanguageCode> Languages present in the engine, in no particular order
     */
    std::vector<LanguageCode> LexEngine::getLanguages() const {
        std::vector<LanguageCode> languages;
        languages.reserve(m_entriesByLang.size());
        for (const auto& [lang, entries] : m_entriesByLang) {
            languages.push_back(lang);
        }
        return languages;
    }

    /**
     * @brief Insert an already-built entry while preserving its id
     *
     * For deserialization only — record() is the entry point for fresh,
     * NLP-token-driven insertion. This method takes an entry already
     * reconstructed by LexEngineSerializer::fromJson(), inserts it into
     * m_entriesByLang/m_lookupIndex/m_byId under the given language,
     * folds its raw_count into m_totalTokens, and advances m_nextId past
     * its id so subsequent record() calls never reuse a restored id.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param entry Already-constructed entry to take ownership of
     * @param lang Language to file the entry under
     * @return LexEntry* Non-owning pointer to the inserted entry
     */
    LexEntry* LexEngine::restoreEntry(LexEntry entry, const LanguageCode& lang) {
        auto owned = std::make_unique<LexEntry>(std::move(entry));
        LexEntry* raw = owned.get();

        m_byId.emplace(raw->id, raw);
        m_lookupIndex.emplace(lookupKey(raw->canonical, lang), raw->id);
        m_totalTokens[lang] += raw->rawCount();
        m_nextId = std::max(m_nextId, static_cast<LexEntryId>(raw->id + 1));
        m_entriesByLang[lang].push_back(std::move(owned));

        return raw;
    }

    /**
     * @brief Resolve a canonical form to its owning entry id
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param form Canonical form to resolve
     * @param lang Language to search within
     * @return LexEntryId Owning entry id, or INVALID_ENTRY_ID if not found
     */
    LexEntryId LexEngine::findEntryId(std::string_view form, const LanguageCode& lang) const {
        const auto it = m_lookupIndex.find(lookupKey(form, lang));
        return it != m_lookupIndex.end() ? it->second : INVALID_ENTRY_ID;
    }

    /**
     * @brief Collect the ids of every entry in a language sharing a given stem
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param stemValue Stem value to match against LexEntry::stem
     * @param lang Language to search within
     * @param excluding Entry id to omit from the results
     * @return std::vector<LexEntryId> Matching entry ids, possibly empty
     */
    std::vector<LexEntryId> LexEngine::findByStem(std::string_view stemValue,
                                                 const LanguageCode& lang,
                                                 LexEntryId excluding) const {
        std::vector<LexEntryId> matches;
        if (stemValue.empty()) return matches;

        const auto it = m_entriesByLang.find(lang);
        if (it == m_entriesByLang.end()) return matches;

        for (const auto& entry : it->second) {
            if (entry->id != excluding && !entry->stem.empty() && entry->stem == stemValue) {
                matches.push_back(entry->id);
            }
        }
        return matches;
    }

} // namespace ADS::LexEngine