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