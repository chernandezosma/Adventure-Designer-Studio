/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#ifndef ADS_CORE_TRANSLATION_SERIALIZER_H
#define ADS_CORE_TRANSLATION_SERIALIZER_H

/**
 * @file TranslationSerializer.h
 * @brief Reads / writes the sibling `<project>.trn` file — the standalone,
 *        per-language store of the game's authored translatable strings
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <filesystem>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

#include "Project.h"

namespace ADS::Core {

    /**
     * @brief In-memory shape of a `.trn` file: langCode -> (stringId -> text).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Only a serialization intermediate — the authoritative in-memory home for
     * the strings is the DataObjects (via Core::TranslationCatalog).
     */
    using TranslationSet = std::map<std::string, std::map<std::string, std::string>>;

    /**
     * @brief Static read/write of the `.trn` translation file.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Follows the LexEngineSerializer contract: returns `bool`, logs via spdlog,
     * never throws, and carries no integrity checksum (the file is meant to be
     * hand-editable by translators).
     */
    class TranslationSerializer {
    public:
        /// On-disk schema version. Bump on an incompatible change.
        static constexpr int kSchemaVersion = 1;

        /**
         * @brief Gather every translatable string in @p project into a set.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Walks Core::TranslationCatalog::enumerate() for every project language
         * (`game.languages.supported`); a string with no text in a language is
         * omitted from that language's map.
         *
         * @param project The project to read
         * @return TranslationSet langCode -> (id -> text)
         */
        static TranslationSet collect(const Project& project);

        /**
         * @brief Push a set's values back onto @p project's DataObjects.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param project The project to mutate
         * @param set     langCode -> (id -> text)
         */
        static void apply(Project& project, const TranslationSet& set);

        /**
         * @brief Serialise a set to the `.trn` JSON document shape.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param set The set
         * @return nlohmann::json `{ "format", "schemaVersion", "languages": { code: { id: text } } }`
         */
        static nlohmann::json toJson(const TranslationSet& set);

        /**
         * @brief Parse a `.trn` JSON document into a set.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param doc The parsed JSON
         * @return TranslationSet The `languages` map, or empty on a version mismatch
         */
        static TranslationSet fromJson(const nlohmann::json& doc);

        /**
         * @brief Write @p project's translations to @p trnPath.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Creates the parent directory. Writes in binary mode (LF endings). If
         * @p project has no translatable text at all, still writes a valid empty
         * document so the file always mirrors the project.
         *
         * @param project The project
         * @param trnPath  Destination `.trn` path
         * @return bool     true on success
         */
        static bool saveToFile(const Project& project, const std::filesystem::path& trnPath);

        /**
         * @brief Load translations from @p trnPath into @p project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * A missing file is not an error (a fresh project has no `.trn` yet) —
         * returns `true` and leaves @p project untouched. Malformed JSON or a
         * schema-version mismatch logs a warning and returns `false`.
         *
         * @param project The project to mutate
         * @param trnPath  Source `.trn` path
         * @return bool     true when the file was absent or loaded cleanly
         */
        static bool loadFromFile(Project& project, const std::filesystem::path& trnPath);
    };

} // namespace ADS::Core

#endif // ADS_CORE_TRANSLATION_SERIALIZER_H
