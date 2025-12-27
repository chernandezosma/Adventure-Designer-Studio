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

#ifndef ADS_JSON_PARSE_EXCEPTION_H
#define ADS_JSON_PARSE_EXCEPTION_H

#include <nlohmann/json.hpp>
#include <string>
#include <format>

namespace ADS::Exceptions {
    using json = nlohmann::json;

    /**
     * @brief Custom JSON parsing exception with file and location tracking
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Enhanced JSON parsing exception that extends nlohmann::json::exception
     * to include the source file path and parsing context (key path within JSON).
     * Provides detailed error information for debugging malformed JSON files.
     *
     * @note Inherits from nlohmann::json::exception for compatibility
     * @see nlohmann::json::exception
     */
    class json_parse_exception : public json::exception {
    private:
        std::string m_file_path;
        std::string m_key_path;
        std::string m_full_message;
        int m_error_id;

    public:
        /**
         * @brief Construct JSON parse exception with detailed context
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates a detailed JSON parsing exception including the file being parsed,
         * the key path within the JSON structure where the error occurred, and the
         * underlying error details from nlohmann::json.
         *
         * @param file_path Path to the JSON file being parsed
         * @param key_path Dot-separated key path within JSON (e.g., "menu.items.0")
         * @param original Original nlohmann::json exception
         */
        json_parse_exception(
            std::string file_path,
            std::string key_path,
            const json::exception& original
        ) : json::exception(original),
            m_file_path(std::move(file_path)),
            m_key_path(std::move(key_path)),
            m_error_id(original.id)
        {
            m_full_message = std::format(
                "[{}] JSON parse error in file '{}' at key '{}': {}",
                m_error_id,
                m_file_path,
                m_key_path.empty() ? "<root>" : m_key_path,
                original.what()
            );
        }

        /**
         * @brief Get the formatted error message
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the complete error message including file path, key path,
         * and original error details in a human-readable format.
         *
         * @return Pointer to null-terminated error message string
         */
        [[nodiscard]] const char* what() const noexcept override {
            return m_full_message.c_str();
        }

        /**
         * @brief Get the file path where the error occurred
         *
         * @return Reference to the JSON file path
         */
        [[nodiscard]] const std::string& getFilePath() const noexcept {
            return m_file_path;
        }

        /**
         * @brief Get the key path within JSON where the error occurred
         *
         * @return Reference to the dot-separated key path
         */
        [[nodiscard]] const std::string& getKeyPath() const noexcept {
            return m_key_path;
        }

        /**
         * @brief Get the nlohmann::json error ID
         *
         * @return Numeric error identifier from nlohmann::json
         */
        [[nodiscard]] int getErrorId() const noexcept {
            return m_error_id;
        }
    };

} // namespace ADS::Exceptions

#endif // ADS_JSON_PARSE_EXCEPTION_H
