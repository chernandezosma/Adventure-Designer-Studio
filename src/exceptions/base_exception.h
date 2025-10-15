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
#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H
#include <utility>

namespace ADS::Exceptions {
    using std::string;
    using std::runtime_error;
    using std::move;

    class BaseException : public std::runtime_error {
        string file;
        int line;
        string full_message;

    public:
        explicit BaseException(const string& msg,
                               string file = __FILE__,
                               const int line = __LINE__): runtime_error(msg), file(std::move(file)), line(line)
        {
            this->full_message = "[" + this->file + ":" + std::to_string(this->line) + "] - " + msg;
        }

        [[nodiscard]] const char* what() const noexcept override
        {
            return this->full_message.c_str();
        }
    };
} // ADS::Exceptions

#endif // BASE_EXCEPTION_H