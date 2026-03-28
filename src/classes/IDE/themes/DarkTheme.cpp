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

        ImGuiStyle& style = ImGui::GetStyle();
        using namespace ADS::IDE::Colors;

        // Structural sizes — applied once here; individual panels must not fight these
        style.FrameBorderSize       = 1.0f;
        style.FrameRounding         = 4.0f;
        style.ScrollbarRounding     = 4.0f;
        style.GrabRounding          = 4.0f;
        style.PopupRounding         = 4.0f;
        style.WindowRounding        = 0.0f;

        // Backgrounds
        style.Colors[ImGuiCol_WindowBg]             = BG0;
        style.Colors[ImGuiCol_ChildBg]              = {0.0f, 0.0f, 0.0f, 0.0f};
        style.Colors[ImGuiCol_PopupBg]              = BG1;
        style.Colors[ImGuiCol_MenuBarBg]            = BG0;

        // Frames (InputText, Slider, Checkbox, etc.)
        style.Colors[ImGuiCol_FrameBg]              = BG1;
        style.Colors[ImGuiCol_FrameBgHovered]       = BG2;
        style.Colors[ImGuiCol_FrameBgActive]        = BG3;

        // Borders
        style.Colors[ImGuiCol_Border]               = BORDER;
        style.Colors[ImGuiCol_BorderShadow]         = {0.0f, 0.0f, 0.0f, 0.0f};
        style.Colors[ImGuiCol_NavHighlight]         = BORDER_HI;

        // Text
        style.Colors[ImGuiCol_Text]                 = TEXT0;
        style.Colors[ImGuiCol_TextDisabled]         = TEXT2;
        style.Colors[ImGuiCol_TextSelectedBg]       = withAlpha(C_SCENE, 0.30f);

        // Collapsible headers / tree nodes
        style.Colors[ImGuiCol_Header]               = BG2;
        style.Colors[ImGuiCol_HeaderHovered]        = BG3;
        style.Colors[ImGuiCol_HeaderActive]         = BG3;

        // Buttons
        style.Colors[ImGuiCol_Button]               = BG2;
        style.Colors[ImGuiCol_ButtonHovered]        = BG3;
        style.Colors[ImGuiCol_ButtonActive]         = withAlpha(C_SCENE, 0.30f);

        // Scrollbar
        style.Colors[ImGuiCol_ScrollbarBg]          = BG0;
        style.Colors[ImGuiCol_ScrollbarGrab]        = BG3;
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = BORDER;
        style.Colors[ImGuiCol_ScrollbarGrabActive]  = BORDER_HI;

        // Interactive indicators
        style.Colors[ImGuiCol_SliderGrab]           = C_SCENE;
        style.Colors[ImGuiCol_SliderGrabActive]     = BORDER_HI;
        style.Colors[ImGuiCol_CheckMark]            = C_OK;

        // Separator
        style.Colors[ImGuiCol_Separator]            = BORDER;
        style.Colors[ImGuiCol_SeparatorHovered]     = BORDER_HI;
        style.Colors[ImGuiCol_SeparatorActive]      = BORDER_HI;

        // Tab bar
        style.Colors[ImGuiCol_Tab]                  = BG1;
        style.Colors[ImGuiCol_TabHovered]           = BG3;
        style.Colors[ImGuiCol_TabActive]            = BG3;
        style.Colors[ImGuiCol_TabUnfocused]         = BG0;
        style.Colors[ImGuiCol_TabUnfocusedActive]   = BG2;

        // Title bar
        style.Colors[ImGuiCol_TitleBg]              = BG0;
        style.Colors[ImGuiCol_TitleBgActive]        = BG1;
        style.Colors[ImGuiCol_TitleBgCollapsed]     = BG0;
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