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
#ifndef ADS_FILE_NOT_OPEN_EXCEPTION_H
#define ADS_FILE_NOT_OPEN_EXCEPTION_H
#include <string>
#include <utility>
#include "../base_exception.h"

namespace ADS::Exceptions {
    /**
     * @brief Exception thrown when a file cannot be opened
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Specialized exception for file opening failures in filesystem operations.
     * Used when a file exists but cannot be opened due to permissions, locks,
     * or other access restrictions.
     *
     * @note Inherits from BaseException for automatic file/line tracking
     * @see BaseException
     */
    class file_not_open_exception final : public BaseException {
    public:
        /**
         * @brief Construct file not open exception
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an exception indicating a file could not be opened.
         *
         * @param msg Description of what file could not be opened and why
         * @param file Source file where exception occurred (auto-captured)
         * @param line Line number where exception occurred (auto-captured)
         */
        explicit file_not_open_exception(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //ADS_FILE_NOT_OPEN_EXCEPTION_H
