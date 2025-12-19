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

#ifndef ADS_ENV_H
#define ADS_ENV_H
#include <ranges>
#include <unordered_map>

namespace ADS {

    using namespace std;

    /**
     * @brief Environment variable manager for .env file configuration
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Oct 2025
     *
     * Handles loading and accessing environment variables from .env files.
     * Parses key-value pairs and optionally sets them as system environment
     * variables. Supports comment lines (starting with #) and quoted values.
     */
    class Environment {

    private:
        /**
         * .env filename
         */
        string filename;

        /**
         * Internal storage for environment variables as key-value pairs
         */
        unordered_map<string, string> environment;

    public:
        /**
         * @brief Construct Environment manager and load .env file
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Oct 2025
         *
         * Creates an Environment object and automatically opens and parses
         * the specified .env file, loading all variables into memory and
         * the system environment.
         *
         * @param filename Path to the .env file (defaults to ".env")
         *
         * @note Constructor automatically calls open() to load the file
         */
        explicit Environment(string filename = ".env");

        /**
         * @brief Load and parse environment variables from .env file
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Oct 2025
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
        bool open();

        /**
         * @brief Retrieve value for a given environment variable key
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Oct 2025
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
        string* get (const string &key);

        /**
         * @brief Return true if debug mode is on (.env). False otherwise
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Nov 2025
         *
         * Searches into the environment for the 'DEBUG' and return its value
         *
         * @return Boolean value
         */
        bool isDebug();
    };

} // ADS

#endif //ADS_ENV_H