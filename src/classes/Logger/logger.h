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

namespace ADS::Core {
    /**
     * @brief Singleton logger facade for spdlog integration
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Centralized logging system that wraps spdlog functionality with both
     * console and rotating file output. Provides application-wide access to
     * a shared logger instance configured with appropriate sinks and formatting.
     *
     * The logger automatically creates daily log files with rotation to prevent
     * excessive disk usage while maintaining comprehensive logging for debugging
     * and monitoring purposes.
     *
     * @note This class follows the singleton pattern with static members only
     * @see spdlog::logger
     */
    class Logger {
    private:
        /**
         * Shared logger instance for application-wide logging.
         * Configured with console and rotating file sinks.
         */
        static std::shared_ptr<spdlog::logger> instance;

        /**
         * @brief Get the current date as a formatted string
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Generates a date string in YYYY-MM-DD format for use in log filenames,
         * allowing automatic daily log file rotation.
         *
         * @return Date string in YYYY-MM-DD format
         */
        static std::string getDateString();

    public:
        /**
         * @brief Initialize the logger with console and file sinks
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Sets up the logging system with configurable output based on DEBUG mode:
         * - DEBUG mode (outputStream == ALL_CONSOLE): All messages to stdout console
         * - Production mode (outputStream empty): Only errors to stderr, all to file
         * - File sink with daily log files (max 5MB per file, 3 files retained)
         *
         * Log files are created in the logs/ directory with date-based naming.
         * Must be called once during application startup before any logging occurs.
         *
         * @param outputStream Configuration string (use ALL_CONSOLE for debug mode, "" for production)
         *
         * @note Subsequent calls have no effect as the instance is already initialized
         */
        static void init(const bool useStdout = false);

        /**
         * @brief Get the logger instance
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Provides access to the shared logger instance for logging operations
         * throughout the application. The logger must be initialized via init()
         * before calling this method.
         *
         * @return Shared pointer to the spdlog logger instance
         *
         * @warning Returns nullptr if init() has not been called
         * @see init()
         */
        static std::shared_ptr<spdlog::logger> getInstance();

    };
}

#endif //ADVENTURE_DESIGNER_STUDIO_LOGGER_H