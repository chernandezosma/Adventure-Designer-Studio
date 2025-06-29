/********************************************************************************
 * Project Name:    Adventure Designer Studio
 * Filename:        path_not_exist_exception.h
 * Description:     
 *
 * Crafted with passion and love in Canary Islands.
 *
 * Copyright (C) 2024 El Estado Web Research & Development. All rights reserved.
 *
 * This document and its contents, including but not limited to code, schemas,
 * images and diagrams are protected by copyright law in Spain and other countries.
 * Unauthorized use, reproduction, distribution, modification, public display, or
 * public performance of any portion of this material is strictly prohibited.
 *
 * Contact Information
 *
 * For permission to use any portion of this material, please contact at
 * El Estado Web Research & Development, S.L. <info@elestadoweb.com>
 ********************************************************************************/
#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H
#include <format>
#include <utility>

namespace ADS::Exceptions {
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
             this->full_message = sprintf ("[%s:%d] - %s",
                this->file,
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
