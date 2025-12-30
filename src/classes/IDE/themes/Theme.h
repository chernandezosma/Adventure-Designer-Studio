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

#ifndef ADS_THEME_H
#define ADS_THEME_H

#include "imgui.h"

namespace ADS::IDE {
    /**
     * @class Theme
     * @brief Base class for IDE theme configuration
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Abstract base class that defines the interface for applying
     * theme configurations to the ImGui style system. Concrete
     * implementations provide specific color schemes and styling.
     */
    class Theme {
    public:
        virtual ~Theme() = default;

        /**
         * @brief Apply the theme to ImGui style
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Applies this theme's color scheme and style settings to the
         * ImGui global style. This method should be called during
         * initialization or when switching themes.
         */
        virtual void apply() = 0;

        /**
         * @brief Get the theme name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns a human-readable name for this theme (e.g., "Dark", "Light").
         *
         * @return const char* Theme name string
         */
        virtual const char* getName() const = 0;
    };
}

#endif // ADS_THEME_H