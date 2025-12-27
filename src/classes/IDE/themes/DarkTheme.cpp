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

#include "DarkTheme.h"

namespace ADS::IDE {
    /**
     * @brief Apply dark theme to ImGui style
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Applies ImGui's default dark color scheme and applies any
     * custom color overrides for the IDE interface including
     * menu dropdown backgrounds and other UI elements.
     */
    void DarkTheme::apply() {
        // Apply ImGui's default dark theme
        ImGui::StyleColorsDark();

        // Get style reference for customization
        ImGuiStyle& style = ImGui::GetStyle();

        // Customize menu dropdown background
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);

        // Additional customizations can be added here
        // Example: style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    }

    /**
     * @brief Get the theme name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * @return const char* Returns "Dark"
     */
    const char* DarkTheme::getName() const {
        return "Dark";
    }
}