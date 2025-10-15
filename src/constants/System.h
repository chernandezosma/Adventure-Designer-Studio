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

#ifndef ADS_DEBUG
    #define ADS_DEBUG
#endif

namespace ADS::Constants {
    // using namespace std;

    class System {
    public:
        static constexpr auto APPLICATION_NAME = "Adventure Studio";
        static constexpr int DEFAULT_X_WIN_SIZE = 1200;
        static constexpr int DEFAULT_Y_WIN_SIZE = 800;

        static constexpr float MIN_WIDTH_RIGHT_DOCK = 400.0f;
        static constexpr float MIN_WIDTH_20_PERCENTAGE = 0.2f;
        static constexpr float MIN_WIDTH_20_UNITS = 20.0f;
        static constexpr float MIN_WIDTH_60_UNITS = 60.0f;
        static constexpr float MIN_WIDTH_80_UNITS = 80.0f;
        static constexpr float MIN_WIDTH_60_PERCENTAGE = 0.6f;
        static constexpr float MIN_WIDTH_40_PERCENTAGE = 0.4f;

        // #ifdef _WIN32
        //         static constexpr char DIRECTORY_SEPARATOR = std::string("\\");
        // #else
        //         static constexpr auto DIRECTORY_SEPARATOR = std::string("/");
        // #endif
    };

}

#endif //ADS_SYSTEMS_H