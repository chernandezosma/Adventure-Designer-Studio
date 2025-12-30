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


#include "im_gui_tools.h"

namespace ADS::MyImGui {

    /**
     * @brief Convert RGB color values to ImGui color format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Converts standard RGB color values (0-255 range) to ImGui's ImVec4
     * format (0.0-1.0 range) by dividing each component by 255.0. This allows
     * using familiar RGB notation (e.g., RGB(255, 128, 0)) in ImGui styling.
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
    ImVec4 Tools::RGB(int r, int g, int b, int a)
    {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    // ADS::imGuiTools::ImGuiTools::ImGuiTools()
    // {
    //
    //     this->fonts.clear();
    //
    // }

    // bool ADS::imGuiTools::ImGuiTools::addFont(std::string fontFileName)
    // {
    //     // if (this->fonts.find())
    //     return true;
    // }

    // ImFont* ADS::imGuiTools::ImGuiTools::loadFonts(std::vector<std::string>& font_files)
    // {
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //     style.FontSizeBase = 20.0f;
    //     io.Fonts->AddFontDefault();
    //     io.Fonts->AddFontFromFileTTF("public/fonts/Raleway/static/Raleway-Light.ttf");
    //     io.Fonts->AddFontFromFileTTF("public/fonts/Raleway/static/Raleway-Medium.ttf");
    //     io.Fonts->AddFontFromFileTTF("public/fonts/Raleway/static/Raleway-Regular.ttf");
    //     ImFont* font = io.Fonts->AddFontFromFileTTF("public/fonts/Raleway/Raleway-VariableFont_wght.ttf");
    //
    //     return font;
}