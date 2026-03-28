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

#include "LightTheme.h"

namespace ADS::IDE {
    /**
     * @brief Apply light theme to ImGui style
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Applies ImGui's default light color scheme and applies any
     * custom color overrides for the IDE interface including
     * menu dropdown backgrounds and other UI elements.
     */
    void LightTheme::apply() {
        // Apply ImGui's default light theme
        ImGui::StyleColorsLight();

        // Get style reference for customization
        ImGuiStyle& style = ImGui::GetStyle();

        // Customize menu dropdown background
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);

        // Additional customizations can be added here
        // Example: style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
    }

    /**
     * @brief Get the theme name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * @return const char* Returns "Light"
     */
    const char* LightTheme::getName() const {
        return "Light";
    }
}