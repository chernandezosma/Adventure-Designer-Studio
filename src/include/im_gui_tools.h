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


#ifndef ADS_IM_GUI_TOOLS_H
#define ADS_IM_GUI_TOOLS_H

#include <string>

#include "imgui.h"

namespace ADS::MyImGui {
    using namespace std;

    class Tools {

    public:
        // ImGuiTools();

        /**
         * @brief Convert RGB color values to ImGui color format
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Converts standard RGB color values (0-255 range) to ImGui's ImVec4
         * format (0.0-1.0 range). Supports optional alpha channel for transparency.
         *
         * @param r Red component (0-255)
         * @param g Green component (0-255)
         * @param b Blue component (0-255)
         * @param a Alpha/opacity component (0-255), default is 255 (fully opaque)
         *
         * @return ImVec4 Color vector in ImGui format with normalized values (0.0-1.0)
         *
         * @note Values outside 0-255 range are not clamped and may produce unexpected results
         */
        static ImVec4 RGB(int r, int g, int b, int a = 255);

    private:
        // bool addFont (std::string fontFileName);

        // ImFont* loadFonts(std::vector<std::string>& font_files);

    };
}

#endif //ADS_IM_GUI_TOOLS_H