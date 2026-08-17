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

#ifndef ADS_LEXICON_LEXICON_SERIALIZER_H
#define ADS_LEXICON_LEXICON_SERIALIZER_H

/**
 * @file LexiconSerializer.h
 * @brief JSON persistence for LexEntry, matching the canonical node structure
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Matches the "Canonical Node Structure" documented in
 * docs/core/lexicon/vocabulary.md: id, lang, canonical, role, types,
 * synonyms, frequency, raw_count, plus an optional synonym_meta block.
 *
 * Fields intentionally NOT persisted (per the same document's "Fields not
 * persisted to JSON" table): dominant_type, type_counts, upos_raw, feats,
 * stem, compiled_token. On load, dominant_type and type_counts are left at
 * their default (None / all-zero) rather than reconstructed, since the
 * observation history required to recompute them is not part of the
 * persisted contract — this is a known, documented limitation carried over
 * from the design document's Open Decisions list, not a bug to silently
 * work around here.
 *
 * @see ADS::Lexicon::LexEntry
 */

#include <nlohmann/json.hpp>

#include "LexEntry.h"

namespace ADS::Lexicon {

    /**
     * @brief Converts LexEntry instances to and from their canonical JSON form
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    class LexiconSerializer {
    public:
        /**
         * @brief Serialise one LexEntry to its canonical JSON representation
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param entry Entry to serialise
         * @return nlohmann::json Object with id, lang, canonical, role, types,
         *         synonyms, frequency, raw_count, and synonym_meta
         */
        [[nodiscard]] static nlohmann::json toJson(const LexEntry& entry);

        /**
         * @brief Deserialise one LexEntry from its canonical JSON representation
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * An absent synonym_meta block is treated per the documented default:
         * every id listed in synonyms is restored as
         * {confidence: 1.0, confirmed: true}.
         *
         * @param json Canonical JSON object previously produced by toJson()
         * @return LexEntry Reconstructed entry — compiled_token is
         *         Token::UNASSIGNED until Lexicon::index() runs again
         */
        [[nodiscard]] static LexEntry fromJson(const nlohmann::json& json);
    };

} // namespace ADS::Lexicon

#endif // ADS_LEXICON_LEXICON_SERIALIZER_H
