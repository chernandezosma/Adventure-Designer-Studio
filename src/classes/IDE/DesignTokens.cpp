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

ImVec4 C_ERROR  = {0.878f, 0.322f, 0.322f, 1.0f};
ImVec4 C_WARN   = {0.910f, 0.643f, 0.290f, 1.0f};
ImVec4 C_OK     = {0.357f, 0.769f, 0.627f, 1.0f};

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
        {"C_ERROR",   &C_ERROR},
        {"C_WARN",    &C_WARN},
        {"C_OK",      &C_OK},
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
    C_ERROR  = {0.878f, 0.322f, 0.322f, 1.0f};
    C_WARN   = {0.910f, 0.643f, 0.290f, 1.0f};
    C_OK     = {0.357f, 0.769f, 0.627f, 1.0f};
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
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[')
            continue;

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

        *it->second = parsed;
        ++loaded;
    }

    spdlog::info("DesignTokens: loaded {} color(s) from '{}'", loaded, path);
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
        "\n"
        "[Backgrounds]\n"
        "BG0       = " << toHexColor(BG0)  << "  # base app background\n"
        "BG1       = " << toHexColor(BG1)  << "  # panel backgrounds\n"
        "BG2       = " << toHexColor(BG2)  << "  # hover state\n"
        "BG3       = " << toHexColor(BG3)  << "  # active / selected\n"
        "\n"
        "[Borders]\n"
        "BORDER    = " << toHexColor(BORDER)    << "  # default border\n"
        "BORDER_HI = " << toHexColor(BORDER_HI) << "  # focused / highlighted border\n"
        "\n"
        "[Text]\n"
        "TEXT0     = " << toHexColor(TEXT0) << "  # primary text\n"
        "TEXT1     = " << toHexColor(TEXT1) << "  # secondary text\n"
        "TEXT2     = " << toHexColor(TEXT2) << "  # muted / disabled labels\n"
        "\n"
        "[EntityAccents]\n"
        "C_SCENE   = " << toHexColor(C_SCENE)  << "  # scenes\n"
        "C_NPC     = " << toHexColor(C_NPC)    << "  # characters / NPCs\n"
        "C_ITEM    = " << toHexColor(C_ITEM)   << "  # objects / items\n"
        "C_PUZZLE  = " << toHexColor(C_PUZZLE) << "  # puzzles\n"
        "C_VAR     = " << toHexColor(C_VAR)    << "  # variables\n"
        "C_AUDIO   = " << toHexColor(C_AUDIO)  << "  # audio\n"
        "\n"
        "[StateIndicators]\n"
        "C_ERROR   = " << toHexColor(C_ERROR) << "  # error\n"
        "C_WARN    = " << toHexColor(C_WARN)  << "  # warning\n"
        "C_OK      = " << toHexColor(C_OK)    << "  # ok / success\n";

    spdlog::info("DesignTokens: saved current theme to '{}'", path);
}

} // namespace ADS::IDE::Colors
