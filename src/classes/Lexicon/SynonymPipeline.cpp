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
 * @file SynonymPipeline.cpp
 * @brief Implementation of SynonymPipeline
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "SynonymPipeline.h"

#include <algorithm>

#include "FallbackNLPBackend.h"

namespace ADS::Lexicon {

    namespace {

        /// Base language subtag from a LanguageCode, e.g. "es_ES" -> "es".
        [[nodiscard]] std::string baseLang(const LanguageCode& lang) noexcept {
            if (const auto pos = lang.find('_'); pos != std::string::npos) return lang.substr(0, pos);
            return lang;
        }

        /// Productive prefixes per language, used by the affix layer to propose a root.
        [[nodiscard]] const std::vector<std::string_view>& prefixesFor(const std::string& base) {
            static const std::vector<std::string_view> es = {"des", "re", "in", "im", "sobre"};
            static const std::vector<std::string_view> en = {"un", "re", "dis", "pre", "over"};
            static const std::vector<std::string_view> de = {"ver", "ent", "ge", "be"};
            static const std::vector<std::string_view> fr = {"re", "de", "in", "pre"};
            static const std::vector<std::string_view> pt = {"des", "re", "in", "im", "sobre"};
            static const std::vector<std::string_view> none = {};

            if (base == "es") return es;
            if (base == "en") return en;
            if (base == "de") return de;
            if (base == "fr") return fr;
            if (base == "pt") return pt;
            return none;
        }

    } // namespace

    LexEntryId SynonymPipeline::affixLayer(std::string_view canonical,
                                            const LanguageCode& lang,
                                            const ILexiconLookup& lookup) {
        constexpr std::size_t kMinRootLength = 3;
        const std::string base = baseLang(lang);

        for (const auto& prefix : prefixesFor(base)) {
            if (canonical.size() > prefix.size() + kMinRootLength && canonical.starts_with(prefix)) {
                const std::string_view root = canonical.substr(prefix.size());
                if (const LexEntryId rootId = lookup.findEntryId(root, lang); rootId != INVALID_ENTRY_ID) {
                    return rootId;
                }
                // Affix detected but the reduced root is not itself a known word
                // (e.g. Spanish "recabar" -> "cabar") — discard rather than
                // propose a low-confidence match, per the design document.
            }
        }
        return INVALID_ENTRY_ID;
    }

    std::vector<SynonymLink> SynonymPipeline::semanticLayer() {
        // Layer 3 (semantic similarity) is deferred — see class docs.
        return {};
    }

    std::vector<SynonymLink> SynonymPipeline::run(std::string_view canonical,
                                                    LexEntryId entryId,
                                                    const LanguageCode& lang,
                                                    const ILexiconLookup& lookup) {
        std::vector<SynonymLink> links;

        if (const LexEntryId rootId = affixLayer(canonical, lang, lookup);
            rootId != INVALID_ENTRY_ID && rootId != entryId) {
            links.push_back(SynonymLink{.target = rootId, .confidence = LAYER1_CONFIDENCE, .confirmed = false});
        }

        // Layer 2 — shared stem grouping (fallback mode, always active since
        // FallbackNLPBackend is currently the only backend implementation).
        const std::string normalisedCanonical = FallbackNLPBackend::normalise(canonical);
        const std::string stemValue = FallbackNLPBackend::stem(normalisedCanonical, lang);
        for (const LexEntryId siblingId : lookup.findByStem(stemValue, lang, entryId)) {
            const bool alreadyLinked = std::any_of(links.begin(), links.end(),
                [siblingId](const SynonymLink& l) { return l.target == siblingId; });
            if (!alreadyLinked) {
                links.push_back(SynonymLink{.target = siblingId, .confidence = LAYER2_CONFIDENCE, .confirmed = false});
            }
        }

        for (auto&& candidate : semanticLayer()) {
            links.push_back(std::move(candidate));
        }

        return links;
    }

} // namespace ADS::Lexicon