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
#ifndef ADS_BASE_EXCEPTION_H
#define ADS_BASE_EXCEPTION_H
#include <format>
#include <stdexcept>
#include <utility>
#include <string>

namespace ADS::Exceptions {
    using std::string;
    using std::runtime_error;
    using std::move;

    /**
     * @brief Base exception class with file and line tracking
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Foundation exception class for all ADS exceptions. Automatically captures
     * the source file and line number where the exception was thrown, formatting
     * them into a descriptive error message. Inherits from std::runtime_error
     * to ensure compatibility with standard exception handling.
     *
     * @note All custom exceptions in the ADS namespace should inherit from this class
     * @see std::runtime_error
     */
    class BaseException : public std::runtime_error {
        /**
         * Source file path where the exception was thrown.
         * Automatically captured via __FILE__ macro.
         */
        string file;

        /**
         * Line number where the exception was thrown.
         * Automatically captured via __LINE__ macro.
         */
        int line;

    public:
        /**
         * @brief Construct exception with message and location information
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an exception with a formatted message including file location
         * and line number. The message format is: "[file:line] - message"
         *
         * @param msg The exception message describing what went wrong
         * @param file Source file where exception occurred (auto-captured via __FILE__)
         * @param line Line number where exception occurred (auto-captured via __LINE__)
         */
        explicit BaseException(std::string_view msg,
                               string file = __FILE__,
                               const int line = __LINE__)
            : runtime_error(std::format("[{}:{}] - {}", std::string_view(file), std::to_string(line), std::string_view(msg))),
              file(std::move(file)), line(line) {}

        /**
         * @brief Get the exception message
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the formatted exception message including file location
         * and line number information. Overrides std::runtime_error::what().
         *
         * @return Pointer to null-terminated string containing the error message
         *
         * @note The pointer remains valid as long as the exception object exists
         */
        [[nodiscard]] const char* what() const noexcept override
        {
            return runtime_error::what();
        }
    };
} // ADS::Exceptions

#endif // ADS_BASE_EXCEPTION_H