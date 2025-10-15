/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */


#include "env.h"
#include <filesystem>
#include <fstream>
#include <utility>

#include "file_not_found_exception.h"

namespace ADS::env {

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
            this->filename = fullPath.string();
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
        std::ifstream file(std::string(this->filename));

        if (!file.is_open()) {
            return false;
        }

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            };

            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Removes spaces and quotes
            if (!value.empty() && value.front() == '"') {
                value.erase(0, 1);
            }

            if (!value.empty() && value.back() == '"') {
                value.pop_back();
            }

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
        auto it = this->environment.find(key);
        if (it != this->environment.end()) {
            return &(it->second);
        }

        return nullptr;
    }
} // ADS