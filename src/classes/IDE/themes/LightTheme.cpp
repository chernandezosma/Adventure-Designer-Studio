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
#include "IDE/DesignTokens.h"

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
        // ImGui's light scheme as the base (fills colours applyStyle() does
        // not touch — DragDropTarget, tables, plots, ModalWindowDimBg …),
        // then swap the ADS tokens to their light values and run the same
        // structural mapping DarkTheme uses. The only difference between the
        // two themes is the token palette.
        ImGui::StyleColorsLight();
        ADS::IDE::Colors::useLightPalette();
        ADS::IDE::Colors::applyStyle(ImGui::GetStyle());
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