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


#include "env.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <utility>

#include "adsString.h"
#include "Core/PathService.h"
#include "../../exceptions/filesystem/file_not_found_exception.h"

namespace ADS {

    /**
     * @brief Construct Environment manager and load .env file
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2025
     *
     * Creates an Environment object and automatically opens and parses
     * the specified .env file, loading all variables into memory and
     * the system environment.
     *
     * @param filename Path to the .env file (defaults to ".env")
     *
     * @note Constructor automatically calls open() to load the file
     */
    Environment::Environment(string filename)
    {
        // Try relative to project root first
        std::filesystem::path projectRoot = PROJECT_ROOT;  // From Method 1 or 2
        std::filesystem::path fullPath = projectRoot / filename;

        if (std::filesystem::exists(fullPath)) {
            this->filename = Core::PathService::toUtf8(fullPath);
        } else {
            // Fall back to direct path
            if (!std::filesystem::exists(filename)) {
                throw Exceptions::file_not_found_exception("File not found: " + filename);
            }
            this->filename = std::move(filename);
        }

        this->open();
    }

    /**
     * @brief Load and parse environment variables from .env file
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2025
     *
     * Opens the .env file and parses its contents line by line. Each line
     * should follow the format KEY=VALUE. Comments (lines starting with #)
     * and empty lines are ignored. Values enclosed in double quotes have
     * the quotes removed. All parsed variables are stored internally and
     * set as system environment variables.
     *
     * @return true if file was successfully opened and parsed
     * @return false if file could not be opened
     *
     * @note Skips malformed lines without '=' separator
     * @note Automatically removes surrounding double quotes from values
     * @note Sets variables in system environment (platform-specific)
     */
    bool Environment::open()
    {
        std::string line;
        std::ifstream file(Core::PathService::pathFromUtf8(this->filename));

        if (!file.is_open()) {
            return false;
        }

        this->m_rawLines.clear();

        while (std::getline(file, line)) {
            // Keep a verbatim copy of every line so set() can rewrite the
            // file without disturbing comments, blank lines or key order.
            this->m_rawLines.push_back(line);

            if (line.empty() || line[0] == '#') {
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            };

            // Trim surrounding whitespace from both sides — "KEY = value" and
            // "KEY=value" must be equivalent, and a stray space in the value
            // (e.g. "PROJECTS_DIR= /home/x") must not become part of a path.
            std::string key   = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Strip one matching pair of surrounding quotes (single or double).
            if (value.size() >= 2 &&
                ((value.front() == '"'  && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            if (key.empty()) {
                continue;
            }

            // Store under the upper-cased key so lookups (which upper-case the
            // requested key) match regardless of how the .env spells it.
            std::transform(key.begin(), key.end(), key.begin(),
                           [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

            this->environment[key] = value;

// #if defined(_WIN32)
//             _putenv_s(key.c_str(), value.c_str());
// #else
//             setenv(key.c_str(), value.c_str(), 1);
// #endif
        }

        file.close();

        return true;
    }

    /**
     * @brief Retrieve value for a given environment variable key
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2025
     *
     * Searches for the specified key in the internal environment map
     * and returns a pointer to its value if found. Returns nullptr if
     * the key does not exist.
     *
     * @param key Reference to the environment variable name to search
     *
     * @return Pointer to the string value if key exists
     * @return nullptr if key is not found
     *
     * @warning The returned pointer is only valid while the Environment
     *          object exists and the internal map is not modified
     * @note Consider using const string& for the key parameter
     */
    string* Environment::get(const string& key)
    {
        string upperKey = key;
        std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(),
            [](unsigned char c){ return std::toupper(c); });

        auto it = this->environment.find(upperKey);
        if (it != this->environment.end()) {
            return &(it->second);
        }

        return nullptr;
    }

    /**
     * @brief Get environment value with default fallback
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns the value associated with the given key from the environment,
     * or the provided default value if the key does not exist. Keys are
     * case-insensitive (automatically converted to uppercase).
     *
     * @param key Environment variable name to retrieve
     * @param defaultValue Value to return if key is not found
     * @return Environment value or default value
     */
    string Environment::getOrDefault(const string& key, const string& defaultValue)
    {
        string* value = get(key);
        return value ? *value : defaultValue;
    }

    /**
     * @brief Set or update a key in the .env file, preserving its layout
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * See the header for the full contract. In short: replace the value of an
     * existing key in place (keeping its line, comments and order) or append
     * `KEY=value` after a blank line, then rewrite the whole file.
     *
     * @param key   Environment variable name (matched case-insensitively)
     * @param value New value to store
     * @return true on success, false if the file could not be written
     */
    bool Environment::set(const string& key, const string& value)
    {
        const auto toUpper = [](string s) {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c) { return std::toupper(c); });
            return s;
        };

        const string wantKey = toUpper(trim(key));

        // Quote only when the value would otherwise be ambiguous on re-read.
        const bool needsQuotes =
            value.find_first_of(" \t#") != std::string::npos;
        const string encoded = needsQuotes ? ("\"" + value + "\"") : value;

        bool replaced = false;
        for (auto& rawLine : this->m_rawLines) {
            const string leftTrimmed = trim(rawLine, " \t");
            if (leftTrimmed.empty() || leftTrimmed[0] == '#') {
                continue;
            }

            const auto eqPos = rawLine.find('=');
            if (eqPos == std::string::npos) {
                continue;
            }

            if (toUpper(trim(rawLine.substr(0, eqPos))) != wantKey) {
                continue;
            }

            // Keep everything up to and including '=', swap the rest.
            rawLine = rawLine.substr(0, eqPos + 1) + encoded;
            replaced = true;
            break;
        }

        if (!replaced) {
            this->m_rawLines.emplace_back("");
            this->m_rawLines.push_back(wantKey + "=" + encoded);
        }

        std::ofstream out(Core::PathService::pathFromUtf8(this->filename), std::ios::trunc);
        if (!out.is_open()) {
            return false;
        }
        for (const auto& rawLine : this->m_rawLines) {
            out << rawLine << '\n';
        }
        out.close();

        this->environment[wantKey] = value;
        return true;
    }

    /**
     * @brief Check if DEBUG environment variable is enabled
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Checks if the DEBUG environment variable exists and is set to a
     * true value. The comparison is case-insensitive and recognizes
     * common representations of boolean true values.
     *
     * @return true if DEBUG exists and is set to "TRUE", "1", "YES", or "ON"
     * @return false if DEBUG is missing, empty, or set to any other value
     *
     * @note The function trims whitespace and performs case-insensitive
     *       comparison for flexibility in .env file formatting
     * @see get() for the underlying environment variable retrieval
     */
    bool Environment::isDebug()
    {
        string* value = this->get("DEBUG");

        if (value == nullptr || value->empty()) {
            return false;
        }

        // Trim whitespace and convert to uppercase
        string trimmedValue = trim(*value);

        // Convert to uppercase
        std::transform(trimmedValue.begin(), trimmedValue.end(), trimmedValue.begin(),
            [](unsigned char c){ return std::toupper(c); });

        // Check for common "true" values
        return trimmedValue == "TRUE" ||
               trimmedValue == "1" ||
               trimmedValue == "YES" ||
               trimmedValue == "ON";
    }
} // ADS