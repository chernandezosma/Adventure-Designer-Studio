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

#include "Project.h"

namespace ADS::Core {

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
         * @version Aug 2026
         *
         * The new Project is fully built locally and only returned on success —
         * a throw leaves any project the caller currently holds untouched.
         *
         * @param path Source `.ads` file
         * @return std::unique_ptr<Project> The rebuilt project
         *
         * @throws Exceptions::file_not_found_exception       if @p path does not exist
         * @throws Exceptions::project_serialization_exception on malformed JSON,
         *         an unsupported schema version, a missing required key, or a
         *         duplicate / dangling id
         */
        static std::unique_ptr<Project> load(const std::filesystem::path& path);
    };

} // namespace ADS::Core

#endif // ADS_CORE_PROJECT_SERIALIZER_H
