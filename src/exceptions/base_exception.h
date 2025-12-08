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

    class BaseException : public std::runtime_error {
        string file;
        int line;

    public:
        explicit BaseException(std::string_view msg,
                               string file = __FILE__,
                               const int line = __LINE__)
            : runtime_error(std::format("[{}:{}] - {}", std::string_view(file), std::to_string(line), std::string_view(msg))),
              file(std::move(file)), line(line) {}

        [[nodiscard]] const char* what() const noexcept override
        {
            return runtime_error::what();
        }
    };
} // ADS::Exceptions

#endif // ADS_BASE_EXCEPTION_H