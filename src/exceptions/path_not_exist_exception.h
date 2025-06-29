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
#ifndef PATH_NOT_EXIST_EXCEPTION_H
#define PATH_NOT_EXIST_EXCEPTION_H
#include <utility>
#include "base_exception.h"

namespace ADS::Exceptions {
    class path_not_exist_exception final : public BaseException {
    public:
        explicit path_not_exist_exception(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //PATH_NOT_EXIST_EXCEPTION_H
