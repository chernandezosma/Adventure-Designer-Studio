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


#ifndef ADS_SYSTEMS_H
#define ADS_SYSTEMS_H
#include <string>

namespace ADS::Constants {
    // using namespace std;

    /**
     * @brief Application-wide system constants and configuration values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Centralized container for system-level constants including application
     * metadata, default window dimensions, and UI layout constraints. These
     * values configure the application's behavior and visual appearance.
     */
    class System {
    public:

        /**
         * Debug mode flag indicating whether the application runs in debug mode.
         * Set at runtime based on environment configuration.
         */
        static bool ADS_DEBUG;

        /**
         * Application display name shown in window title and UI.
         */
        static constexpr auto APPLICATION_NAME = "Adventure Studio";

        /**
         * Path to the ImGui configuration file for persistent UI state.
         */
        static constexpr auto CONFIG_FILE = "imgui.ini";

        /**
         * Default main window width in pixels.
         */
        static constexpr int DEFAULT_X_WIN_SIZE = 1200;

        /**
         * Default main window height in pixels.
         */
        static constexpr int DEFAULT_Y_WIN_SIZE = 800;

        /**
         * Minimum width in pixels for the right dock panel in the IDE layout.
         */
        static constexpr float MIN_WIDTH_RIGHT_DOCK = 400.0f;

        /**
         * 20% width ratio constant for proportional layout calculations.
         */
        static constexpr float MIN_WIDTH_20_PERCENTAGE = 0.2f;

        /**
         * Minimum width of 20 pixels for UI elements.
         */
        static constexpr float MIN_WIDTH_20_UNITS = 20.0f;

        /**
         * Minimum width of 60 pixels for UI elements.
         */
        static constexpr float MIN_WIDTH_60_UNITS = 60.0f;

        /**
         * Minimum width of 80 pixels for UI elements.
         */
        static constexpr float MIN_WIDTH_80_UNITS = 80.0f;

        /**
         * 60% width ratio constant for proportional layout calculations.
         */
        static constexpr float MIN_WIDTH_60_PERCENTAGE = 0.6f;

        /**
         * 40% width ratio constant for proportional layout calculations.
         */
        static constexpr float MIN_WIDTH_40_PERCENTAGE = 0.4f;

        // #ifdef _WIN32
        //         static constexpr char DIRECTORY_SEPARATOR = std::string("\\");
        // #else
        //         static constexpr auto DIRECTORY_SEPARATOR = std::string("/");
        // #endif
    };

}

#endif //ADS_SYSTEMS_H