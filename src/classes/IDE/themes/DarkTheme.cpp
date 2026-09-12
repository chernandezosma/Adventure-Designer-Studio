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
#include "IDE/DesignTokens.h"

namespace ADS::IDE {
    /**
     * @brief Apply dark theme to ImGui style
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Applies ImGui's default dark color scheme as a base, then overrides
     * it completely with the ADS DesignTokens palette so that all widgets
     * inherit the correct colours without needing per-panel push/pop pairs.
     */
    void DarkTheme::apply() {
        ImGui::StyleColorsDark();
        ADS::IDE::Colors::useDarkPalette();
        ADS::IDE::Colors::applyStyle(ImGui::GetStyle());
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