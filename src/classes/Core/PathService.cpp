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

#include "PathService.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <string_view>
#include <system_error>

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

#include "spdlog/spdlog.h"

// PathService is a pure static helper, so it logs every operation at the
// `debug` level (visible when DEBUG=true): inputs in, resolved path out.
// Anything that had to fall back (no HOME) or that reports a clash is a
// `warn`.

namespace ADS::Core {

    namespace {

        /// Longest slug this service will emit (folder-name friendliness).
        constexpr std::size_t kMaxSlugLength = 64;

        /**
         * @brief Case-insensitively test a slug against the Windows reserved
         *        device names that are illegal as file/folder stems.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param slug A candidate slug (already lower-case ASCII)
         * @return bool True if @p slug is CON, PRN, AUX, NUL, COM1-9 or LPT1-9
         */
        bool isReservedDeviceName(std::string_view slug)
        {
            static constexpr std::array<std::string_view, 3> kBare{"con", "prn", "aux"};
            if (std::find(kBare.begin(), kBare.end(), slug) != kBare.end()) {
                return true;
            }
            if (slug == "nul") {
                return true;
            }
            if (slug.size() == 4 && (slug.starts_with("com") || slug.starts_with("lpt"))
                && slug[3] >= '1' && slug[3] <= '9') {
                return true;
            }
            return false;
        }

    } // namespace

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
    std::filesystem::path PathService::homeDir()
    {
        if (const char* sdlHome = SDL_GetUserFolder(SDL_FOLDER_HOME); sdlHome && *sdlHome) {
            // SDL owns this pointer — copy, do not free.
            const std::filesystem::path p = pathFromUtf8(sdlHome);
            spdlog::debug("PathService::homeDir -> '{}' (SDL_FOLDER_HOME)", toUtf8(p));
            return p;
        }
        if (const char* home = std::getenv("HOME"); home && *home) {
            const std::filesystem::path p = pathFromUtf8(home);
            spdlog::debug("PathService::homeDir -> '{}' ($HOME)", toUtf8(p));
            return p;
        }
        if (const char* userProfile = std::getenv("USERPROFILE"); userProfile && *userProfile) {
            const std::filesystem::path p = pathFromUtf8(userProfile);
            spdlog::debug("PathService::homeDir -> '{}' ($USERPROFILE)", toUtf8(p));
            return p;
        }
        std::error_code ec;
        const std::filesystem::path cwd = std::filesystem::current_path(ec);
        const std::filesystem::path p = ec ? std::filesystem::path{"."} : cwd;
        spdlog::warn("PathService::homeDir: no home directory in the environment — "
                     "falling back to '{}'", toUtf8(p));
        return p;
    }

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
    std::filesystem::path PathService::projectsRoot(std::string_view overrideDir)
    {
        const std::filesystem::path resolved = expandUser(overrideDir);
        if (!resolved.empty()) {
            spdlog::debug("PathService::projectsRoot(override='{}') -> '{}'",
                          overrideDir, toUtf8(resolved));
            return resolved;
        }
        const std::filesystem::path fallback = homeDir() / "ADS-Projects";
        spdlog::debug("PathService::projectsRoot(override='') -> '{}' (default)",
                      toUtf8(fallback));
        return fallback;
    }

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
    std::filesystem::path PathService::pathFromUtf8(std::string_view utf8)
    {
        std::filesystem::path p(
            std::u8string(reinterpret_cast<const char8_t*>(utf8.data()), utf8.size()));
        spdlog::debug("PathService::pathFromUtf8('{}') -> '{}'", utf8, toUtf8(p));

        return p;
    }

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
    std::string PathService::toUtf8(const std::filesystem::path& path)
    {
        const std::u8string u8 = path.u8string();
        return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
    }

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
    std::filesystem::path PathService::expandUser(std::string_view raw)
    {
        // Trim leading / trailing ASCII whitespace — a stray space from a
        // ".env" value must never end up as a path component.
        const auto begin = raw.find_first_not_of(" \t\r\n");
        if (begin == std::string_view::npos) {
            spdlog::debug("PathService::expandUser('{}') -> '' (blank)", raw);
            return {};
        }
        const auto end = raw.find_last_not_of(" \t\r\n");
        const std::string_view s = raw.substr(begin, end - begin + 1);

        // Expand a leading "~" (whole value) or "~/" prefix to the home dir.
        std::filesystem::path result;
        if (s == "~") {
            result = homeDir();
        } else if (s.size() >= 2 && s[0] == '~' && (s[1] == '/' || s[1] == '\\')) {
            result = homeDir() / pathFromUtf8(s.substr(2));
        } else {
            result = pathFromUtf8(s);
        }
        spdlog::debug("PathService::expandUser('{}') -> '{}'", raw, toUtf8(result));
        return result;
    }

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
    std::string PathService::slugify(std::string_view name)
    {
        std::string slug;
        slug.reserve(std::min(name.size(), kMaxSlugLength));

        bool pendingDash = false;
        for (unsigned char ch : name) {
            const bool isLower = ch >= 'a' && ch <= 'z';
            const bool isUpper = ch >= 'A' && ch <= 'Z';
            const bool isDigit = ch >= '0' && ch <= '9';

            if (isLower || isDigit) {
                if (pendingDash && !slug.empty()) {
                    slug.push_back('-');
                }
                pendingDash = false;
                slug.push_back(static_cast<char>(ch));
            } else if (isUpper) {
                if (pendingDash && !slug.empty()) {
                    slug.push_back('-');
                }
                pendingDash = false;
                slug.push_back(static_cast<char>(ch - 'A' + 'a'));
            } else {
                // Any run of separators / punctuation / non-ASCII collapses to one dash.
                pendingDash = true;
            }

            if (slug.size() >= kMaxSlugLength) {
                break;
            }
        }

        while (!slug.empty() && slug.back() == '-') {
            slug.pop_back();
        }

        if (slug.empty()) {
            slug = "project";
        } else if (isReservedDeviceName(slug)) {
            slug.insert(slug.begin(), '_');
        }
        spdlog::debug("PathService::slugify('{}') -> '{}'", name, slug);
        return slug;
    }

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
    std::filesystem::path PathService::projectFile(const std::filesystem::path& root,
                                                  std::string_view projectName)
    {
        const std::string slug = slugify(projectName);
        const std::filesystem::path file = root / slug / (slug + ".ads");
        spdlog::debug("PathService::projectFile(root='{}', name='{}') -> '{}'",
                      toUtf8(root), projectName, toUtf8(file));
        return file;
    }

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
    bool PathService::projectFolderExists(const std::filesystem::path& projectDir)
    {
        std::error_code ec;
        if (!std::filesystem::exists(projectDir, ec) || ec) {
            spdlog::debug("PathService::projectFolderExists('{}') -> false (does not exist)",
                          toUtf8(projectDir));
            return false;
        }
        if (!std::filesystem::is_directory(projectDir, ec) || ec) {
            // A non-directory sitting where the project folder would go is a clash.
            spdlog::warn("PathService::projectFolderExists('{}') -> true "
                         "(a non-directory is in the way)", toUtf8(projectDir));
            return true;
        }
        const bool nonEmpty = !std::filesystem::is_empty(projectDir, ec) && !ec;
        spdlog::debug("PathService::projectFolderExists('{}') -> {} ({})",
                      toUtf8(projectDir), nonEmpty,
                      nonEmpty ? "non-empty directory" : "empty directory");
        return nonEmpty;
    }

} // namespace ADS::Core
