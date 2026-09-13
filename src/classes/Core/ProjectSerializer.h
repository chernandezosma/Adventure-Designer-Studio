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

#ifndef ADS_CORE_PROJECT_SERIALIZER_H
#define ADS_CORE_PROJECT_SERIALIZER_H

/**
 * @file ProjectSerializer.h
 * @brief Reads and writes a Core::Project to a `.ads` JSON file
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "Project.h"

namespace ADS::Core {

    /**
     * @brief One entity that failed to load and was skipped.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Produced by ProjectSerializer::load() when an individual entity's JSON
     * is malformed (missing/invalid field, duplicate id) but the rest of the
     * file is structurally sound. The entity is simply omitted from the
     * rebuilt Project rather than aborting the whole load.
     */
    struct LoadWarning {
        /// Human-readable entity kind, e.g. "Scene", "Character", "Item", "State", "Chain".
        std::string entityKind;

        /// Best-effort id/name pulled from the entity's JSON before the failure, for display.
        std::string identifier;

        /// The underlying exception message explaining why the entity was skipped.
        std::string reason;
    };

    /**
     * @brief Result of a (possibly partial) ProjectSerializer::load().
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * `project` always contains every entity that parsed successfully.
     * `warnings` lists every entity that was skipped; it is empty on a clean load.
     */
    struct LoadResult {
        /// The rebuilt project, containing every successfully-parsed entity.
        std::unique_ptr<Project> project;

        /// One entry per entity skipped due to a per-entity parse failure.
        std::vector<LoadWarning> warnings;
    };

    /**
     * @brief Whole-project `.ads` save / load via nlohmann_json.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Serialises the project name, its Data::GameData and every DataObject
     * collection, plus the LexEngine (through LexEngineSerializer). On load a
     * brand-new Project is built collection-by-collection through the public
     * Project::addX(id, name) methods, so the entity adapters are rebuilt with
     * valid back-pointers and the DataObject pointees are never moved.
     */
    class ProjectSerializer {
    public:
        /// Current on-disk schema version. Bump on an incompatible change.
        static constexpr int kSchemaVersion = 1;

        /**
         * @brief Write @p project to @p path as pretty-printed JSON.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param project Project to serialise
         * @param path    Destination file (overwritten if it exists)
         *
         * @throws Exceptions::project_serialization_exception if the file cannot
         *         be opened for writing
         */
        static void save(const Project& project, const std::filesystem::path& path);

        /**
         * @brief Rebuild a Project from the `.ads` file at @p path.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * The new Project is fully built locally and only returned on success —
         * a throw leaves any project the caller currently holds untouched.
         *
         * A per-entity failure (missing/invalid field, duplicate id) does not
         * abort the whole load: that single entity is skipped, recorded in
         * LoadResult::warnings, and every other entity still loads normally.
         * Whole-file failures still throw, since there is nothing left to
         * salvage: unreadable/missing file, malformed top-level JSON, an
         * unsupported schema version, or a checksum mismatch (the file was
         * edited outside the editor, so no entity's data can be trusted).
         *
         * @param path Source `.ads` file
         * @return LoadResult The rebuilt project plus any skipped-entity warnings
         *
         * @throws Exceptions::file_not_found_exception       if @p path does not exist
         * @throws Exceptions::project_serialization_exception on malformed JSON,
         *         an unsupported schema version, or a checksum mismatch
         */
        static LoadResult load(const std::filesystem::path& path);
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_SERIALIZER_H
