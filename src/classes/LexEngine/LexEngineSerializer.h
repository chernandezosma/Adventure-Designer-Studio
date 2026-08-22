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

#ifndef ADS_LEXENGINE_LEXENGINE_SERIALIZER_H
#define ADS_LEXENGINE_LEXENGINE_SERIALIZER_H

/**
 * @file LexEngineSerializer.h
 * @brief JSON persistence for LexEntry, matching the canonical node structure
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Matches the "Canonical Node Structure" documented in
 * docs/core/lexengine/vocabulary.md: id, lang, canonical, role, types,
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
 * @see ADS::LexEngine::LexEntry
 */

#include <filesystem>

#include <nlohmann/json.hpp>

#include "LexEntry.h"
#include "LexEngine.h"

namespace ADS::LexEngine {

    /**
     * @brief Converts LexEntry instances to and from their canonical JSON form
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    class LexEngineSerializer {
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
         *         Token::UNASSIGNED until LexEngine::index() runs again
         */
        [[nodiscard]] static LexEntry fromJson(const nlohmann::json& json);

        /**
         * @brief Serialise an entire LexEngine (every language) to one JSON document
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param engine Engine to serialise
         * @return nlohmann::json Object with "version" (currently 1) and a flat
         *         "entries" array covering every language — each element is the
         *         same shape produced by toJson(const LexEntry&)
         */
        [[nodiscard]] static nlohmann::json toJson(const LexEngine& engine);

        /**
         * @brief Restore every entry from a whole-engine JSON document into engine
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Reconstructs each entry via fromJson() and inserts it through
         * LexEngine::restoreEntry(), preserving ids. Only "version": 1
         * documents are currently supported.
         *
         * @param engine Engine to populate — existing entries are left untouched
         * @param document Document previously produced by toJson(const LexEngine&)
         */
        static void fromJson(LexEngine& engine, const nlohmann::json& document);

        /**
         * @brief Write an engine's whole vocabulary to a JSON file on disk
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param engine Engine to serialise
         * @param path Destination file path — overwritten if it already exists
         * @return bool True on success, false if the file could not be written
         */
        [[nodiscard]] static bool saveToFile(const LexEngine& engine, const std::filesystem::path& path);

        /**
         * @brief Load a whole vocabulary from a JSON file on disk into engine
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param engine Engine to populate
         * @param path Source file path
         * @return bool True on success, false if the file is missing, unreadable, or malformed
         */
        [[nodiscard]] static bool loadFromFile(LexEngine& engine, const std::filesystem::path& path);
    };

} // namespace ADS::LexEngine

#endif // ADS_LEXENGINE_LEXENGINE_SERIALIZER_H
