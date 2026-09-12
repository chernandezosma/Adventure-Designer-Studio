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

#pragma once

#include "imgui.h"

/**
 * @file DesignTokens.h
 * @brief Runtime colour palette and design constants for the ADS IDE.
 *
 * Colours are non-const ImVec4 variables so they can be loaded from
 * an external INI file (public/colors.ini) at startup without recompiling.
 * All panels and renderers should reference these names rather than
 * using inline literals.
 *
 * Typical initialisation (call once after the ImGui context is ready):
 * @code
 *   ADS::IDE::Colors::loadFromFile("public/colors.ini");
 * @endcode
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

namespace ADS::IDE::Colors {

    // ------------------------------------------------------------------
    // Backgrounds
    // ------------------------------------------------------------------

    /** @brief #0f1117 — base application background */
    extern ImVec4 BG0;
    /** @brief #161b24 — panel backgrounds */
    extern ImVec4 BG1;
    /** @brief #1e2533 — hover state */
    extern ImVec4 BG2;
    /** @brief #252d3e — active selection */
    extern ImVec4 BG3;

    // ------------------------------------------------------------------
    // Borders
    // ------------------------------------------------------------------

    /** @brief #2e3a50 — default border */
    extern ImVec4 BORDER;
    /** @brief #3d4f6e — highlighted / focused border */
    extern ImVec4 BORDER_HI;

    // ------------------------------------------------------------------
    // Text
    // ------------------------------------------------------------------

    /** @brief #e8e6df — primary text */
    extern ImVec4 TEXT0;
    /** @brief #b8b4a8 — secondary text */
    extern ImVec4 TEXT1;
    /** @brief #5a6275 — muted labels */
    extern ImVec4 TEXT2;

    // ------------------------------------------------------------------
    // Entity type accents
    // ------------------------------------------------------------------

    /** @brief #7b9dff — Scenes */
    extern ImVec4 C_SCENE;
    /** @brief #d4667a — NPCs / Characters */
    extern ImVec4 C_NPC;
    /** @brief #5bc4a0 — Items / Objects */
    extern ImVec4 C_ITEM;
    /** @brief #e8a44a — Puzzles */
    extern ImVec4 C_PUZZLE;
    /** @brief #b07cff — Variables */
    extern ImVec4 C_VAR;
    /** @brief #f08a5d — Audio */
    extern ImVec4 C_AUDIO;
    /** @brief #adda68 — States */
    extern ImVec4 C_STATE;
    /** @brief #5dc9d3 — State chains */
    extern ImVec4 C_CHAIN;

    // ------------------------------------------------------------------
    // State indicators
    // ------------------------------------------------------------------

    /** @brief #e05252 — error */
    extern ImVec4 C_ERROR;
    /** @brief #e8a44a — warning */
    extern ImVec4 C_WARN;
    /** @brief #5bc4a0 — ok / success */
    extern ImVec4 C_OK;

    // ------------------------------------------------------------------
    // Dialog chrome
    // ------------------------------------------------------------------

    /** @brief #2f5fb0 — modal caption bar fill (distinct from the dialog body) */
    extern ImVec4 C_DIALOG_TITLE;

    /** @brief #1c3a63 — docked-panel caption + dock-tab fill (deeper blue than
     *  the dialogs, so modals still read as foreground) */
    extern ImVec4 C_PANEL_TITLE;

    /** @brief #f0f3f7 — text drawn on the blue caption bars; near-white in both
     *  the light and the dark palette because the caption fill is always dark */
    extern ImVec4 C_CAPTION_TEXT;

    // ------------------------------------------------------------------
    // Helpers
    // ------------------------------------------------------------------

    /**
     * @brief Return a copy of @p color with its alpha channel replaced by @p alpha.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param color  Source colour.
     * @param alpha  Desired alpha in [0, 1].
     * @return ImVec4 The same RGB values with the new alpha.
     */
    inline ImVec4 withAlpha(ImVec4 color, float alpha)
    {
        return {color.x, color.y, color.z, alpha};
    }

    // ------------------------------------------------------------------
    // INI persistence
    // ------------------------------------------------------------------

    /**
     * @brief Load colour tokens from an INI file, overriding defaults.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * The file format is one token per line: @c TOKEN_NAME = #RRGGBBAA
     * Lines starting with @c # or @c ; are comments.
     * Lines starting with @c [ are section headers (organisational only).
     * Unknown token names are silently ignored.
     * If the file cannot be opened the built-in defaults are preserved.
     *
     * @param path Path to the INI file (relative to the working directory).
     */
    void loadFromFile(const char* path);

    /**
     * @brief Write the current colour token values to an INI file.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Produces a human-readable file that can be edited in any text editor
     * and reloaded with @c loadFromFile.
     *
     * @param path Destination path (relative to the working directory).
     */
    void saveToFile(const char* path);

    /**
     * @brief Restore all colour tokens to their built-in default values.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    void resetToDefaults();

    // ------------------------------------------------------------------
    // Theme palettes
    // ------------------------------------------------------------------

    /**
     * @brief Switch the neutral tokens (BG*, BORDER*, TEXT*, entity accents)
     *        to their dark values.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Restores the palette that was active before the first useLightPalette()
     * call (which may include values loaded from colors.ini). A no-op when the
     * dark palette was never left. Called by DarkTheme::apply().
     */
    void useDarkPalette();

    /**
     * @brief Switch the neutral tokens to a light palette.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The first call snapshots the current (dark) values so useDarkPalette()
     * can restore them exactly. The blue caption tokens (C_DIALOG_TITLE /
     * C_PANEL_TITLE) and C_CAPTION_TEXT are deliberately left unchanged — the
     * caption bars stay branded blue with near-white text in both themes.
     * Called by LightTheme::apply().
     */
    void useLightPalette();

    /**
     * @brief Map the current colour tokens onto the ImGui style.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The single structural mapping shared by DarkTheme and LightTheme —
     * each calls StyleColorsDark()/Light() and use{Dark,Light}Palette() first,
     * then this. Because the mapping is identical, the theme's look is entirely
     * a function of the token values.
     *
     * @param style The ImGui style to write into (usually ImGui::GetStyle()).
     */
    void applyStyle(ImGuiStyle& style);

} // namespace ADS::IDE::Colors
