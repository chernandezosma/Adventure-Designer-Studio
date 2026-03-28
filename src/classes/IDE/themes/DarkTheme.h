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

#ifndef ADS_DARK_THEME_H
#define ADS_DARK_THEME_H

#include "Theme.h"

namespace ADS::IDE {
    /**
     * @class DarkTheme
     * @brief Dark color scheme for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Implements a dark theme using ImGui's built-in dark color scheme
     * as the foundation. Can be extended to customize specific colors
     * for menu dropdowns, panels, and other UI elements.
     */
    class DarkTheme : public Theme {
    public:
        /**
         * @brief Apply dark theme to ImGui style
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Applies ImGui's default dark color scheme and applies any
         * custom color overrides for the IDE interface.
         */
        void apply() override;

        /**
         * @brief Get the theme name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * @return const char* Returns "Dark"
         */
        const char* getName() const override;
    };
}

#endif // ADS_DARK_THEME_H