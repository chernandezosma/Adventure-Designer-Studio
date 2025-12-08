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


#ifndef ADS_LOGGER_H
#define ADS_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ADS::Core {
    class Logger {

    private:
        static std::shared_ptr<spdlog::logger> instance;

        /**
         * Get the current date as a formatted string (YYYY-MM-DD)
         * @return Date string for filename
         */
        static std::string getDateString();

    public:
        /**
         * Initialize the logger with console and file sinks
         */
        static void init();

        /**
         * Get the logger instance
         * @return Shared pointer to the spdlog logger
         */
        static std::shared_ptr<spdlog::logger> getInstance();

    };
}

#endif //ADVENTURE_DESIGNER_STUDIO_LOGGER_H