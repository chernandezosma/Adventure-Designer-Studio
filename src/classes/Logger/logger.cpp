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


#include "logger.h"
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

std::shared_ptr<spdlog::logger> ADS::Core::Logger::instance = nullptr;

/**
 * @brief Generate current date string in YYYY-MM-DD format
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Nov 2025
 *
 * Creates a formatted date string for use in log filenames,
 * allowing logs to be organized by date.
 *
 * @return std::string Current date in YYYY-MM-DD format
 */
std::string ADS::Core::Logger::getDateString()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d");
    return ss.str();
}

/**
 * @brief Initialize the logger with console and file output sinks
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Nov 2025
 *
 * Sets up a dual-sink logger that outputs to both console (with color)
 * and a rotating log file. The log file is named with the current date
 * and rotates when it reaches 5MB. Creates the logs/ directory if needed.
 *
 * @see getInstance()
 */
void ADS::Core::Logger::init()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    std::string log_filename = "logs/" + getDateString() + "_ads.log";
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_filename, 1048576 * 5, 3);
    file_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    instance = std::make_shared<spdlog::logger>("ADS", sinks.begin(), sinks.end());

    spdlog::set_default_logger(instance);
    spdlog::set_level(spdlog::level::debug);
}

/**
 * @brief Retrieve the logger instance for app-wide use
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Nov 2025
 *
 * Returns the initialized logger instance that has been configured
 * with both console and file output sinks. This instance is set as
 * the default logger for spdlog.
 *
 * @return std::shared_ptr<spdlog::logger> Shared pointer to the logger instance
 *
 * @warning The logger must be initialized via init() before calling this method
 *
 * @see init()
 */
std::shared_ptr<spdlog::logger> ADS::Core::Logger::getInstance()
{
    return instance;
}