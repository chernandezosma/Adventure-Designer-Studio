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


#ifndef NAVIGATIONCONSTANTS_H
#define NAVIGATIONCONSTANTS_H
#include <string>

namespace ADS::Constants::Navigation {
    constexpr std::string_view MENU_FILE_NEW = "hanfleFileNew";
    constexpr std::string_view MENU_FILE_OPEN = "handleFileOpen";
    constexpr std::string_view MENU_FILE_SAVE = "handleFileSave";
    constexpr std::string_view MENU_FILE_EXIT = "handleFileExit";

    constexpr std::string_view MENU_VIEW_RESET_LAYOUT = "handleViewResetLayout";
    constexpr std::string_view MENU_VIEW_DARK_THEME = "handleViewDarkTheme";
    constexpr std::string_view MENU_VIEW_LIGHT_THEME = "handleViewLightTheme";

    constexpr std::string_view MENU_HELP_ABOUT = "handleHelpAbout";

}

#endif //NAVIGATIONCONSTANTS_H