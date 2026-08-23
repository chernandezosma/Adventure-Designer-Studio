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

#ifndef ADS_CORE_PATH_SERVICE_H
#define ADS_CORE_PATH_SERVICE_H

/**
 * @file PathService.h
 * @brief The single home for platform-sensitive filesystem-path logic
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <filesystem>
#include <string>
#include <string_view>

namespace ADS::Core {

    /**
     * @brief Portable helpers for locating and naming project files on disk.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * All members are static — this class holds no state. It is the only place
     * in the codebase that touches OS-specific path behaviour (the user's home
     * directory, UTF-8 to native path conversion). Everything else works in
     * terms of `std::filesystem::path`, which already normalises separators.
     *
     * Deliberately free of any dependency on `Core::App` so it can link into
     * the test-only `ads_core` library: callers pass the `.env` `PROJECTS_DIR`
     * value into projectsRoot() rather than this class reading it.
     */
    class PathService {
    public:
        /**
         * @brief Resolve the current user's home directory.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Tries SDL3's `SDL_GetUserFolder(SDL_FOLDER_HOME)` first, then the
         * `HOME` and `USERPROFILE` environment variables, and finally the
         * process working directory. The result is never empty.
         *
         * @return std::filesystem::path Absolute path to the home directory
         */
        static std::filesystem::path homeDir();

        /**
         * @brief Resolve the root directory under which new projects are created.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param overrideDir The `.env` `PROJECTS_DIR` value, or empty to use
         *                    the default `homeDir() / "ADS-Projects"`
         * @return std::filesystem::path The projects root (not created here)
         */
        static std::filesystem::path projectsRoot(std::string_view overrideDir = {});

        /**
         * @brief Build a std::filesystem::path from a UTF-8 encoded string.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Uses the `char8_t` path constructor, which treats the bytes as UTF-8
         * and converts to the platform's native encoding (UTF-16 on Windows).
         * This is why no `#ifdef` is needed for correct non-ASCII paths.
         *
         * @param utf8 A UTF-8 encoded path string
         * @return std::filesystem::path The equivalent native path
         */
        static std::filesystem::path pathFromUtf8(std::string_view utf8);

        /**
         * @brief Turn a std::filesystem::path back into a UTF-8 encoded string.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * The inverse of pathFromUtf8(). Uses path::u8string(), which always
         * yields UTF-8 bytes regardless of platform. Prefer this over
         * path::string() for anything other than a same-process, same-locale
         * debug print — string() re-encodes through the OS ANSI codepage on
         * Windows, which corrupts non-ASCII characters (e.g. an accented
         * username).
         *
         * @param path The path to encode
         * @return std::string UTF-8 encoded path string
         */
        static std::string toUtf8(const std::filesystem::path& path);

        /**
         * @brief Normalise a user-supplied directory string into a real path.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Trims surrounding whitespace and expands a leading `~` or `~/` to
         * homeDir(). Anything else is passed through pathFromUtf8(). Use this
         * on values that come from a `.env` key or a hand-typed field before
         * treating them as a filesystem location. An empty / whitespace-only
         * input yields an empty path.
         *
         * @param raw The raw directory string (e.g. `~/ADS-Projects`)
         * @return std::filesystem::path The resolved path
         */
        static std::filesystem::path expandUser(std::string_view raw);

        /**
         * @brief Turn a project display name into a filesystem-safe stem.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Lower-cases ASCII letters, replaces every run of non-`[a-z0-9]`
         * characters with a single `-`, trims leading/trailing `-`, caps the
         * length, and guards the Windows reserved device names (CON, PRN, AUX,
         * NUL, COM1-9, LPT1-9) by prefixing `_`. Non-ASCII is dropped — the
         * human-readable name lives on in `game.title`. An empty or
         * all-punctuation input yields `"project"`.
         *
         * @param name The project display name
         * @return std::string A safe folder/file stem, always non-empty
         */
        static std::string slugify(std::string_view name);

        /**
         * @brief Compose the default `.ads` file path for a project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * `<root> / <slug> / <slug>.ads`, where `<slug>` is slugify(@p projectName).
         *
         * @param root        The projects root (see projectsRoot())
         * @param projectName The project display name
         * @return std::filesystem::path The target file path (not created here)
         */
        static std::filesystem::path projectFile(const std::filesystem::path& root,
                                                 std::string_view projectName);

        /**
         * @brief Test whether a project folder already exists and is non-empty.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Used by the New Project dialog to refuse overwriting an existing
         * project. An empty directory is treated as free to use.
         *
         * @param projectDir The candidate `<root>/<slug>` directory
         * @return bool True if @p projectDir exists and contains at least one entry
         */
        static bool projectFolderExists(const std::filesystem::path& projectDir);
    };

} // namespace ADS::Core

#endif // ADS_CORE_PATH_SERVICE_H
