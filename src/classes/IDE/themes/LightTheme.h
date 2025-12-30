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

#ifndef ADS_LIGHT_THEME_H
#define ADS_LIGHT_THEME_H

#include "Theme.h"

namespace ADS::IDE {
    /**
     * @class LightTheme
     * @brief Light color scheme for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Implements a light theme using ImGui's built-in light color scheme
     * as the foundation. Can be extended to customize specific colors
     * for menu dropdowns, panels, and other UI elements.
     */
    class LightTheme : public Theme {
    public:
        /**
         * @brief Apply light theme to ImGui style
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Applies ImGui's default light color scheme and applies any
         * custom color overrides for the IDE interface.
         */
        void apply() override;

        /**
         * @brief Get the theme name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * @return const char* Returns "Light"
         */
        const char* getName() const override;
    };
}

#endif // ADS_LIGHT_THEME_H