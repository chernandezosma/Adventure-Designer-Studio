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
#ifndef ADS_PATH_NOT_EXIST_EXCEPTION_H
#define ADS_PATH_NOT_EXIST_EXCEPTION_H
#include <string>
#include <utility>
#include "../base_exception.h"

namespace ADS::Exceptions {
    /**
     * @brief Exception thrown when a filesystem path does not exist
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Specialized exception for non-existent path errors in filesystem operations.
     * Used when attempting to access a directory or file path that does not exist
     * in the filesystem.
     *
     * @note Inherits from BaseException for automatic file/line tracking
     * @see BaseException
     */
    class path_not_exist_exception final : public BaseException {
    public:
        /**
         * @brief Construct path not exist exception
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an exception indicating a filesystem path does not exist.
         *
         * @param msg Description of what path was not found and context
         * @param file Source file where exception occurred (auto-captured)
         * @param line Line number where exception occurred (auto-captured)
         */
        explicit path_not_exist_exception(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //ADS_PATH_NOT_EXIST_EXCEPTION_H
