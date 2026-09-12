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

/**
 * @file DesignTokens.cpp
 * @brief Runtime colour token definitions and INI persistence.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include "DesignTokens.h"
#include "spdlog/spdlog.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace ADS::IDE::Colors {

// ---------------------------------------------------------------------------
// Variable definitions — default values match the original constexpr palette
// ---------------------------------------------------------------------------

ImVec4 BG0      = {0.059f, 0.067f, 0.090f, 1.0f};
ImVec4 BG1      = {0.086f, 0.106f, 0.141f, 1.0f};
ImVec4 BG2      = {0.118f, 0.145f, 0.200f, 1.0f};
ImVec4 BG3      = {0.145f, 0.176f, 0.243f, 1.0f};

ImVec4 BORDER    = {0.180f, 0.227f, 0.314f, 1.0f};
ImVec4 BORDER_HI = {0.239f, 0.310f, 0.431f, 1.0f};

ImVec4 TEXT0    = {0.910f, 0.902f, 0.875f, 1.0f};
ImVec4 TEXT1    = {0.722f, 0.706f, 0.659f, 1.0f};
ImVec4 TEXT2    = {0.353f, 0.384f, 0.459f, 1.0f};

ImVec4 C_SCENE  = {0.482f, 0.616f, 1.000f, 1.0f};
ImVec4 C_NPC    = {0.831f, 0.400f, 0.478f, 1.0f};
ImVec4 C_ITEM   = {0.357f, 0.769f, 0.627f, 1.0f};
ImVec4 C_PUZZLE = {0.910f, 0.643f, 0.290f, 1.0f};
ImVec4 C_VAR    = {0.690f, 0.486f, 1.000f, 1.0f};
ImVec4 C_AUDIO  = {0.941f, 0.541f, 0.365f, 1.0f};
ImVec4 C_STATE  = {0.678f, 0.855f, 0.408f, 1.0f};
ImVec4 C_CHAIN  = {0.365f, 0.788f, 0.827f, 1.0f};

ImVec4 C_ERROR  = {0.878f, 0.322f, 0.322f, 1.0f};
ImVec4 C_WARN   = {0.910f, 0.643f, 0.290f, 1.0f};
ImVec4 C_OK     = {0.357f, 0.769f, 0.627f, 1.0f};

ImVec4 C_DIALOG_TITLE = {0.184f, 0.373f, 0.690f, 1.0f};
ImVec4 C_PANEL_TITLE  = {0.110f, 0.227f, 0.388f, 1.0f};
ImVec4 C_CAPTION_TEXT = {0.941f, 0.953f, 0.965f, 1.0f};

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

static std::string trim(const std::string& s)
{
    auto first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

// Parse "#RRGGBBAA" or "#RRGGBB" into an ImVec4 (float [0,1] per channel).
// Returns false if the string is not a valid hex colour.
static bool parseHexColor(const std::string& hex, ImVec4& out)
{
    if (hex.empty() || hex[0] != '#') return false;
    const std::size_t len = hex.size();
    if (len != 7 && len != 9) return false;

    auto hexByte = [&](std::size_t pos) -> int {
        char buf[3] = { hex[pos], hex[pos + 1], '\0' };
        return static_cast<int>(std::strtol(buf, nullptr, 16));
    };

    out.x = hexByte(1) / 255.0f;
    out.y = hexByte(3) / 255.0f;
    out.z = hexByte(5) / 255.0f;
    out.w = (len == 9) ? hexByte(7) / 255.0f : 1.0f;
    return true;
}

// Build the lookup table: token name → pointer to the variable.
static std::unordered_map<std::string, ImVec4*> buildColorMap()
{
    return {
        {"BG0",       &BG0},
        {"BG1",       &BG1},
        {"BG2",       &BG2},
        {"BG3",       &BG3},
        {"BORDER",    &BORDER},
        {"BORDER_HI", &BORDER_HI},
        {"TEXT0",     &TEXT0},
        {"TEXT1",     &TEXT1},
        {"TEXT2",     &TEXT2},
        {"C_SCENE",   &C_SCENE},
        {"C_NPC",     &C_NPC},
        {"C_ITEM",    &C_ITEM},
        {"C_PUZZLE",  &C_PUZZLE},
        {"C_VAR",     &C_VAR},
        {"C_AUDIO",   &C_AUDIO},
        {"C_STATE",   &C_STATE},
        {"C_CHAIN",   &C_CHAIN},
        {"C_ERROR",   &C_ERROR},
        {"C_WARN",    &C_WARN},
        {"C_OK",      &C_OK},
        {"C_DIALOG_TITLE", &C_DIALOG_TITLE},
        {"C_PANEL_TITLE",  &C_PANEL_TITLE},
        {"C_CAPTION_TEXT", &C_CAPTION_TEXT},
    };
}

// Format an ImVec4 as "#RRGGBBAA".
static std::string toHexColor(const ImVec4& c)
{
    char buf[10];
    std::snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x",
        static_cast<int>(c.x * 255.0f + 0.5f),
        static_cast<int>(c.y * 255.0f + 0.5f),
        static_cast<int>(c.z * 255.0f + 0.5f),
        static_cast<int>(c.w * 255.0f + 0.5f));
    return buf;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void resetToDefaults()
{
    BG0      = {0.059f, 0.067f, 0.090f, 1.0f};
    BG1      = {0.086f, 0.106f, 0.141f, 1.0f};
    BG2      = {0.118f, 0.145f, 0.200f, 1.0f};
    BG3      = {0.145f, 0.176f, 0.243f, 1.0f};
    BORDER    = {0.180f, 0.227f, 0.314f, 1.0f};
    BORDER_HI = {0.239f, 0.310f, 0.431f, 1.0f};
    TEXT0    = {0.910f, 0.902f, 0.875f, 1.0f};
    TEXT1    = {0.722f, 0.706f, 0.659f, 1.0f};
    TEXT2    = {0.353f, 0.384f, 0.459f, 1.0f};
    C_SCENE  = {0.482f, 0.616f, 1.000f, 1.0f};
    C_NPC    = {0.831f, 0.400f, 0.478f, 1.0f};
    C_ITEM   = {0.357f, 0.769f, 0.627f, 1.0f};
    C_PUZZLE = {0.910f, 0.643f, 0.290f, 1.0f};
    C_VAR    = {0.690f, 0.486f, 1.000f, 1.0f};
    C_AUDIO  = {0.941f, 0.541f, 0.365f, 1.0f};
    C_STATE  = {0.678f, 0.855f, 0.408f, 1.0f};
    C_CHAIN  = {0.365f, 0.788f, 0.827f, 1.0f};
    C_ERROR  = {0.878f, 0.322f, 0.322f, 1.0f};
    C_WARN   = {0.910f, 0.643f, 0.290f, 1.0f};
    C_OK     = {0.357f, 0.769f, 0.627f, 1.0f};
    C_DIALOG_TITLE = {0.184f, 0.373f, 0.690f, 1.0f};
    C_PANEL_TITLE  = {0.110f, 0.227f, 0.388f, 1.0f};
    C_CAPTION_TEXT = {0.941f, 0.953f, 0.965f, 1.0f};
}

// ---------------------------------------------------------------------------
// Light palette
// ---------------------------------------------------------------------------

namespace {
    // Values parsed from the [Light*] sections of colors.ini; layered over
    // lightPaletteDefaults() by useLightPalette().
    std::unordered_map<std::string, ImVec4> g_lightOverrides;

    // The built-in light palette. The dark palette lives in the plain token
    // globals (and the non-[Light] sections of colors.ini); this is its light
    // counterpart, overridable per-token from colors.ini's [Light*] sections.
    std::unordered_map<std::string, ImVec4> lightPaletteDefaults()
    {
        return {
            {"BG0",            {0.937f, 0.941f, 0.949f, 1.0f}},
            {"BG1",            {1.000f, 1.000f, 1.000f, 1.0f}},
            {"BG2",            {0.914f, 0.922f, 0.933f, 1.0f}},
            {"BG3",            {0.847f, 0.859f, 0.878f, 1.0f}},
            {"BORDER",         {0.780f, 0.800f, 0.824f, 1.0f}},
            {"BORDER_HI",      {0.545f, 0.588f, 0.647f, 1.0f}},
            {"TEXT0",          {0.114f, 0.129f, 0.153f, 1.0f}},
            {"TEXT1",          {0.325f, 0.353f, 0.396f, 1.0f}},
            {"TEXT2",          {0.545f, 0.573f, 0.616f, 1.0f}},
            {"C_SCENE",        {0.145f, 0.365f, 0.792f, 1.0f}},
            {"C_NPC",          {0.729f, 0.204f, 0.376f, 1.0f}},
            {"C_ITEM",         {0.078f, 0.529f, 0.384f, 1.0f}},
            {"C_PUZZLE",       {0.690f, 0.427f, 0.086f, 1.0f}},
            {"C_VAR",          {0.427f, 0.286f, 0.769f, 1.0f}},
            {"C_AUDIO",        {0.737f, 0.290f, 0.106f, 1.0f}},
            {"C_STATE",        {0.286f, 0.529f, 0.169f, 1.0f}},
            {"C_CHAIN",        {0.086f, 0.529f, 0.573f, 1.0f}},
            {"C_ERROR",        {0.769f, 0.149f, 0.149f, 1.0f}},
            {"C_WARN",         {0.686f, 0.404f, 0.000f, 1.0f}},
            {"C_OK",           {0.078f, 0.529f, 0.384f, 1.0f}},
            {"C_PANEL_TITLE",  {0.788f, 0.855f, 0.949f, 1.0f}},
            {"C_DIALOG_TITLE", {0.706f, 0.804f, 0.933f, 1.0f}},
            {"C_CAPTION_TEXT", {0.114f, 0.129f, 0.153f, 1.0f}},
        };
    }
}

void loadFromFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        spdlog::warn("DesignTokens: cannot open '{}', using built-in defaults", path);
        return;
    }

    auto colorMap = buildColorMap();
    int  loaded   = 0;
    bool inLight  = false; // true while inside a [Light*] section
    std::string line;

    g_lightOverrides.clear();

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line[0] == '[') {
            std::string section = line;
            std::transform(section.begin(), section.end(), section.begin(),
                [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            inLight = section.find("light") != std::string::npos;
            continue;
        }

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key   = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));

        // Strip a trailing inline comment (e.g. "#0f1117ff  # base app background"),
        // searching from index 1 so the leading '#' of the hex value itself is kept.
        auto commentPos = value.find_first_of("#;", 1);
        if (commentPos != std::string::npos)
            value = trim(value.substr(0, commentPos));

        // Convert key to uppercase for case-insensitive matching
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char c){ return static_cast<char>(std::toupper(c)); });

        auto it = colorMap.find(key);
        if (it == colorMap.end()) {
            spdlog::debug("DesignTokens: unknown token '{}', skipped", key);
            continue;
        }

        ImVec4 parsed;
        if (!parseHexColor(value, parsed)) {
            spdlog::warn("DesignTokens: '{}' has invalid color value '{}'", key, value);
            continue;
        }

        if (inLight) {
            g_lightOverrides[key] = parsed; // applied later by useLightPalette()
        } else {
            *it->second = parsed;           // dark palette — the live tokens
        }
        ++loaded;
    }

    spdlog::info("DesignTokens: loaded {} color(s) from '{}' ({} light override(s))",
                 loaded, path, g_lightOverrides.size());
}

void saveToFile(const char* path)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("DesignTokens: cannot write '{}'", path);
        return;
    }

    file <<
        "# ADS IDE Color Theme\n"
        "# Edit any value and restart the application to apply changes.\n"
        "# Format: TOKEN_NAME = #RRGGBBAA  (hexadecimal, 00-ff per channel)\n"
        "# Lines starting with # or ; are comments.\n"
        "#\n"
        "# Two areas below: DARK THEME and LIGHT THEME. Section names are\n"
        "# organisational only — any section whose name contains \"light\" feeds\n"
        "# the light theme, every other section feeds the dark theme.\n"
        "\n"
        "# ====================================================================\n"
        "#  DARK THEME\n"
        "# ====================================================================\n"
        "\n"
        "[Dark.Backgrounds]\n"
        "BG0       = " << toHexColor(BG0)  << "  # base app background\n"
        "BG1       = " << toHexColor(BG1)  << "  # panel backgrounds\n"
        "BG2       = " << toHexColor(BG2)  << "  # hover state\n"
        "BG3       = " << toHexColor(BG3)  << "  # active / selected\n"
        "\n"
        "[Dark.Borders]\n"
        "BORDER    = " << toHexColor(BORDER)    << "  # default border\n"
        "BORDER_HI = " << toHexColor(BORDER_HI) << "  # focused / highlighted border\n"
        "\n"
        "[Dark.Text]\n"
        "TEXT0     = " << toHexColor(TEXT0) << "  # primary text\n"
        "TEXT1     = " << toHexColor(TEXT1) << "  # secondary text\n"
        "TEXT2     = " << toHexColor(TEXT2) << "  # muted / disabled labels\n"
        "\n"
        "[Dark.EntityAccents]\n"
        "C_SCENE   = " << toHexColor(C_SCENE)  << "  # scenes\n"
        "C_NPC     = " << toHexColor(C_NPC)    << "  # characters / NPCs\n"
        "C_ITEM    = " << toHexColor(C_ITEM)   << "  # objects / items\n"
        "C_PUZZLE  = " << toHexColor(C_PUZZLE) << "  # puzzles\n"
        "C_VAR     = " << toHexColor(C_VAR)    << "  # variables\n"
        "C_AUDIO   = " << toHexColor(C_AUDIO)  << "  # audio\n"
        "C_STATE   = " << toHexColor(C_STATE)  << "  # states\n"
        "C_CHAIN   = " << toHexColor(C_CHAIN)  << "  # state chains\n"
        "\n"
        "[Dark.StateIndicators]\n"
        "C_ERROR   = " << toHexColor(C_ERROR) << "  # error\n"
        "C_WARN    = " << toHexColor(C_WARN)  << "  # warning\n"
        "C_OK      = " << toHexColor(C_OK)    << "  # ok / success\n"
        "\n"
        "[Dark.Dialogs]\n"
        "C_DIALOG_TITLE = " << toHexColor(C_DIALOG_TITLE) << "  # modal caption bar\n"
        "C_PANEL_TITLE  = " << toHexColor(C_PANEL_TITLE)  << "  # docked panel caption + dock tab\n"
        "C_CAPTION_TEXT = " << toHexColor(C_CAPTION_TEXT) << "  # text on the caption bars\n";

    // Light palette — every [Light...] section feeds useLightPalette().
    const auto lightDefaults = lightPaletteDefaults();
    const auto L = [&](const char* name) {
        if (const auto o = g_lightOverrides.find(name); o != g_lightOverrides.end()) {
            return o->second;
        }
        return lightDefaults.at(name);
    };

    file <<
        "\n"
        "# ====================================================================\n"
        "#  LIGHT THEME\n"
        "# ====================================================================\n"
        "\n"
        "[Light.Backgrounds]\n"
        "BG0       = " << toHexColor(L("BG0"))  << "  # base app background\n"
        "BG1       = " << toHexColor(L("BG1"))  << "  # panel backgrounds\n"
        "BG2       = " << toHexColor(L("BG2"))  << "  # hover state\n"
        "BG3       = " << toHexColor(L("BG3"))  << "  # active / selected\n"
        "\n"
        "[Light.Borders]\n"
        "BORDER    = " << toHexColor(L("BORDER"))    << "  # default border\n"
        "BORDER_HI = " << toHexColor(L("BORDER_HI")) << "  # focused / highlighted border\n"
        "\n"
        "[Light.Text]\n"
        "TEXT0     = " << toHexColor(L("TEXT0")) << "  # primary text\n"
        "TEXT1     = " << toHexColor(L("TEXT1")) << "  # secondary text\n"
        "TEXT2     = " << toHexColor(L("TEXT2")) << "  # muted / disabled labels\n"
        "\n"
        "[Light.EntityAccents]\n"
        "C_SCENE   = " << toHexColor(L("C_SCENE"))  << "  # scenes\n"
        "C_NPC     = " << toHexColor(L("C_NPC"))    << "  # characters / NPCs\n"
        "C_ITEM    = " << toHexColor(L("C_ITEM"))   << "  # objects / items\n"
        "C_PUZZLE  = " << toHexColor(L("C_PUZZLE")) << "  # puzzles\n"
        "C_VAR     = " << toHexColor(L("C_VAR"))    << "  # variables\n"
        "C_AUDIO   = " << toHexColor(L("C_AUDIO"))  << "  # audio\n"
        "C_STATE   = " << toHexColor(L("C_STATE"))  << "  # states\n"
        "C_CHAIN   = " << toHexColor(L("C_CHAIN"))  << "  # state chains\n"
        "\n"
        "[Light.StateIndicators]\n"
        "C_ERROR   = " << toHexColor(L("C_ERROR")) << "  # error\n"
        "C_WARN    = " << toHexColor(L("C_WARN"))  << "  # warning\n"
        "C_OK      = " << toHexColor(L("C_OK"))    << "  # ok / success\n"
        "\n"
        "[Light.Dialogs]\n"
        "C_DIALOG_TITLE = " << toHexColor(L("C_DIALOG_TITLE")) << "  # modal caption bar\n"
        "C_PANEL_TITLE  = " << toHexColor(L("C_PANEL_TITLE"))  << "  # docked panel caption + dock tab\n"
        "C_CAPTION_TEXT = " << toHexColor(L("C_CAPTION_TEXT")) << "  # text on the caption bars\n";

    spdlog::info("DesignTokens: saved current theme to '{}'", path);
}

// ---------------------------------------------------------------------------
// Theme palettes
// ---------------------------------------------------------------------------

namespace {
    // Snapshot of the neutral tokens taken the first time the light palette is
    // applied, so switching back to dark restores exactly what was loaded
    // (defaults or colors.ini overrides), not a hard-coded guess.
    bool                   g_darkSnapshotTaken = false;
    std::unordered_map<std::string, ImVec4> g_darkSnapshot;

    // Tokens the palette swap touches — including the caption bar / tab fill
    // and its text: the dark theme uses deep blue with near-white text, the
    // light theme a pale blue with dark ink.
    const char* const kPaletteTokens[] = {
        "BG0", "BG1", "BG2", "BG3", "BORDER", "BORDER_HI",
        "TEXT0", "TEXT1", "TEXT2",
        "C_SCENE", "C_NPC", "C_ITEM", "C_PUZZLE", "C_VAR", "C_AUDIO",
        "C_STATE", "C_CHAIN", "C_ERROR", "C_WARN", "C_OK",
        "C_PANEL_TITLE", "C_DIALOG_TITLE", "C_CAPTION_TEXT",
    };
}

void useDarkPalette()
{
    if (!g_darkSnapshotTaken) {
        return; // never left the dark palette — tokens are already dark
    }
    const auto map = buildColorMap();
    for (const char* name : kPaletteTokens) {
        const auto snap = g_darkSnapshot.find(name);
        const auto live = map.find(name);
        if (snap != g_darkSnapshot.end() && live != map.end()) {
            *live->second = snap->second;
        }
    }
}

void useLightPalette()
{
    const auto map = buildColorMap();

    if (!g_darkSnapshotTaken) {
        for (const char* name : kPaletteTokens) {
            if (const auto it = map.find(name); it != map.end()) {
                g_darkSnapshot[name] = *it->second;
            }
        }
        g_darkSnapshotTaken = true;
    }

    // Built-in light palette, then colors.ini [Light*] overrides on top.
    auto light = lightPaletteDefaults();
    for (const auto& [name, colour] : g_lightOverrides) {
        light[name] = colour;
    }
    for (const auto& [name, colour] : light) {
        if (const auto it = map.find(name); it != map.end()) {
            *it->second = colour;
        }
    }
}

// ---------------------------------------------------------------------------
// Style mapping
// ---------------------------------------------------------------------------

void applyStyle(ImGuiStyle& style)
{
    // Structural sizes — applied once here; individual panels must not fight these
    style.FrameBorderSize   = 1.0f;
    style.FrameRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 4.0f;
    style.PopupRounding     = 4.0f;
    style.WindowRounding    = 0.0f;

    // Backgrounds
    style.Colors[ImGuiCol_WindowBg]  = BG0;
    style.Colors[ImGuiCol_ChildBg]   = {0.0f, 0.0f, 0.0f, 0.0f};
    style.Colors[ImGuiCol_PopupBg]   = BG1;
    style.Colors[ImGuiCol_MenuBarBg] = BG0;

    // Frames (InputText, Slider, Checkbox, etc.)
    style.Colors[ImGuiCol_FrameBg]        = BG1;
    style.Colors[ImGuiCol_FrameBgHovered] = BG2;
    style.Colors[ImGuiCol_FrameBgActive]  = BG3;

    // Borders
    style.Colors[ImGuiCol_Border]       = BORDER;
    style.Colors[ImGuiCol_BorderShadow] = {0.0f, 0.0f, 0.0f, 0.0f};
    style.Colors[ImGuiCol_NavHighlight] = BORDER_HI;

    // Text
    style.Colors[ImGuiCol_Text]           = TEXT0;
    style.Colors[ImGuiCol_TextDisabled]   = TEXT2;
    style.Colors[ImGuiCol_TextSelectedBg] = withAlpha(C_SCENE, 0.30f);

    // Collapsible headers / tree nodes
    style.Colors[ImGuiCol_Header]        = BG2;
    style.Colors[ImGuiCol_HeaderHovered] = BG3;
    style.Colors[ImGuiCol_HeaderActive]  = BG3;

    // Buttons
    style.Colors[ImGuiCol_Button]        = BG2;
    style.Colors[ImGuiCol_ButtonHovered] = BG3;
    style.Colors[ImGuiCol_ButtonActive]  = withAlpha(C_SCENE, 0.30f);

    // Scrollbar
    style.Colors[ImGuiCol_ScrollbarBg]          = BG0;
    style.Colors[ImGuiCol_ScrollbarGrab]        = BG3;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = BORDER;
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = BORDER_HI;

    // Interactive indicators
    style.Colors[ImGuiCol_SliderGrab]       = C_SCENE;
    style.Colors[ImGuiCol_SliderGrabActive] = BORDER_HI;
    style.Colors[ImGuiCol_CheckMark]        = C_OK;

    // Separator
    style.Colors[ImGuiCol_Separator]        = BORDER;
    style.Colors[ImGuiCol_SeparatorHovered] = BORDER_HI;
    style.Colors[ImGuiCol_SeparatorActive]  = BORDER_HI;

    // Tab bar — dock tabs share the panel caption colour (C_PANEL_TITLE),
    // dimmed by opacity for the inactive / unfocused states (kept fairly
    // opaque so the pale light-theme blue still reads as a tab).
    style.Colors[ImGuiCol_Tab]                = withAlpha(C_PANEL_TITLE, 0.70f);
    style.Colors[ImGuiCol_TabHovered]         = C_PANEL_TITLE;
    style.Colors[ImGuiCol_TabActive]          = C_PANEL_TITLE;
    style.Colors[ImGuiCol_TabUnfocused]       = withAlpha(C_PANEL_TITLE, 0.55f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = withAlpha(C_PANEL_TITLE, 0.90f);

    // Title bar — docked panels use C_PANEL_TITLE; modal dialogs override it
    // with C_DIALOG_TITLE in ModalScaffold. Caption text is forced to
    // C_CAPTION_TEXT by the panel / dialog begin wrappers.
    style.Colors[ImGuiCol_TitleBg]          = C_PANEL_TITLE;
    style.Colors[ImGuiCol_TitleBgActive]    = C_PANEL_TITLE;
    style.Colors[ImGuiCol_TitleBgCollapsed] = C_PANEL_TITLE;
}

} // namespace ADS::IDE::Colors
