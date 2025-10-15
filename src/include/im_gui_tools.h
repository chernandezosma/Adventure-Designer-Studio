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
#include <vector>

#include "imgui.h"

namespace ADS::imGuiTools {
    using namespace std;

    class ImGuiTools {

        /**
         * Vector with fonts that would be loaded.
         */
        std::vector<string> fonts;

    public:
        ImGuiTools();

    private:
        bool addFont (std::string fontFileName);

        ImFont* loadFonts(std::vector<std::string>& font_files);

    };
}

#endif //ADS_IM_GUI_TOOLS_H