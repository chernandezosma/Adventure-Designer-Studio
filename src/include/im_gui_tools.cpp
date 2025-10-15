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


ADS::imGuiTools::ImGuiTools::ImGuiTools()
{

    this->fonts.clear();

}

bool ADS::imGuiTools::ImGuiTools::addFont(std::string fontFileName)
{
    // if (this->fonts.find())
}

ImFont* ADS::imGuiTools::ImGuiTools::loadFonts(std::vector<std::string>& font_files)
{
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