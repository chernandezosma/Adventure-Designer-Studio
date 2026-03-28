/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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
#ifndef ADS_FILE_NOT_OPEN_EXCEPTION_H
#define ADS_FILE_NOT_OPEN_EXCEPTION_H
#include <utility>
#include "../base_exception.h"

namespace ADS::Exceptions {
    /**
     * @brief Exception thrown when a file cannot be found
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Specialized exception for file not found errors in filesystem operations.
     * Used when attempting to access, open, or read a file that does not exist
     * at the specified path.
     *
     * @note Inherits from BaseException for automatic file/line tracking
     * @see BaseException
     */
    class file_not_found_exception final : public BaseException {
    public:
        /**
         * @brief Construct file not found exception
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an exception indicating a file could not be found.
         *
         * @param msg Description of what file was not found and context
         * @param file Source file where exception occurred (auto-captured)
         * @param line Line number where exception occurred (auto-captured)
         */
        explicit file_not_found_exception(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //ADS_FILE_NOT_OPEN_EXCEPTION_H
