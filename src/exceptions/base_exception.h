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
#include <format>
#include <utility>
#include <iostream>

namespace ADS::Exceptions {
    using namespace std;
    using std::string;
    using std::format;
    using std::runtime_error;
    using std::move;

    class BaseException : public std::runtime_error {
        string file;
        int line;
        string full_message;

    public:
        explicit BaseException(const string &msg,
                               string file = __FILE__,
                               const int line = __LINE__): runtime_error(msg), file(move(file)), line(line) {
#if __cplusplus >= 202302L
            // You might format the message here or in what()
            this->full_message = format(
                "[{}:{}] - {}",
                this->file,
                this->line,
                msg
            );
#else
            this->full_message = sprintf((char*)"[%s:%d] - %s", 
                this->file.c_str(),
                this->line,
                msg
            );
            cerr << this->full_message << endl;
#endif
        }

        [[nodiscard]] const char *what() const noexcept override {
            return this->full_message.c_str();
        }
    };
} // ADS::Exceptions

#endif // BASE_EXCEPTION_H
