/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
 */


/**
 * @file NavigationConstants.h
 * @brief String constants for menu and navigation action identifiers
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
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