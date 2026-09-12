/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#ifndef ADS_PROJECT_SERIALIZATION_EXCEPTION_H
#define ADS_PROJECT_SERIALIZATION_EXCEPTION_H

#include <string>
#include <utility>

#include "../base_exception.h"

namespace ADS::Exceptions {

    /**
     * @brief Thrown when a project cannot be written to or rebuilt from an
     *        `.ads` file (bad schema version, duplicate/dangling id, missing
     *        required key, unwritable path).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    class project_serialization_exception final : public BaseException {
    public:
        /**
         * @brief Construct the exception with a message and source location
         * @param msg Description of the serialization failure
         * @param file Source file where the exception was thrown (auto-captured)
         * @param line Line number where the exception was thrown (auto-captured)
         */
        explicit project_serialization_exception(const std::string& msg,
                                                 std::string file = __FILE__,
                                                 const int line = __LINE__)
            : BaseException(msg, std::move(file), line) {}
    };

} // namespace ADS::Exceptions

#endif // ADS_PROJECT_SERIALIZATION_EXCEPTION_H
