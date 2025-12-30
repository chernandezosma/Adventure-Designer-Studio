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
#ifndef ADS_IMGUI_CREATE_WINDOW_EXCEPTION_H
#define ADS_IMGUI_CREATE_WINDOW_EXCEPTION_H

#include <string>
#include <utility>
#include "base_exception.h"

namespace ADS::Imgui::Exceptions {
    /**
     * @brief Exception thrown when ImGui window initialization fails
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Specialized exception for ImGui window creation and initialization failures.
     * Used when SDL window creation, ImGui context initialization, or backend
     * setup encounters errors during application startup.
     *
     * @note Inherits from BaseException for automatic file/line tracking
     * @see ADS::Exceptions::BaseException
     */
    class window_initialization_exception final : public ADS::Exceptions::BaseException {
    public:
        /**
         * @brief Construct window initialization exception
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an exception indicating ImGui window initialization failed.
         *
         * @param msg Description of what initialization step failed and why
         * @param file Source file where exception occurred (auto-captured)
         * @param line Line number where exception occurred (auto-captured)
         */
        explicit window_initialization_exception(const std::string &msg, std::string file = __FILE__, const int line = __LINE__):
            BaseException(msg, std::move(file), line) {}
    };
} // ADS::Exceptions

#endif //ADS_IMGUI_CREATE_WINDOW_EXCEPTION_H
