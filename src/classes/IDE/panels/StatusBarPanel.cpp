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

#include "StatusBarPanel.h"
#include "IDE/DesignTokens.h"
#include "imgui.h"
#include <format>
#include <string>
#include <vector>

namespace ADS::IDE::Panels {

using namespace ADS::IDE::Colors;

namespace {
    /// App version used when the .env does not set ADS_VERSION.
    constexpr const char* kFallbackVersion = "0.4.2";
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

StatusBarPanel::StatusBarPanel()
    : BasePanel("StatusBar")
{
    // Read once — the version never changes at runtime.
    m_version = m_environment
        ? m_environment->getOrDefault("ADS_VERSION", kFallbackVersion)
        : std::string{kFallbackVersion};
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

/**
 * @brief Set the display name of the active project.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param name Project name shown in the status bar.
 */
void StatusBarPanel::setProjectName(const std::string& name)   { m_projectName = name; }

/**
 * @brief Set the name of the currently open file.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param filename File name (not full path) shown in the status bar.
 */
void StatusBarPanel::setActiveFile(const std::string& filename) { m_activeFile  = filename; }

/**
 * @brief Set the editor cursor position displayed in the status bar.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param line 1-based line number.
 * @param col  1-based column number.
 */
void StatusBarPanel::setCursorPosition(int line, int col)       { m_line = line; m_col = col; }

/**
 * @brief Set the entity and diagnostic counts shown on the right side.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param scenes   Number of scenes in the project.
 * @param npcs     Number of NPCs in the project.
 * @param warnings Number of compiler / lint warnings.
 * @param errors   Number of compiler / lint errors.
 */
void StatusBarPanel::setCounts(int scenes, int npcs, int warnings, int errors)
{
    m_scenes   = scenes;
    m_npcs     = npcs;
    m_warnings = warnings;
    m_errors   = errors;
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------

/**
 * @brief Return the fixed height of the status bar in pixels.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @return float Always 22.0f as per the design specification.
 */
float StatusBarPanel::getHeight() const
{
    // Computed each call so DPI / font changes are reflected automatically.
    // WindowPadding.y (4 px) × 2 gives comfortable vertical breathing room.
    return ImGui::GetTextLineHeight() + ImGui::GetStyle().WindowPadding.y * 2.0f;
}

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------

/**
 * @brief Render the status bar for the current frame.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Positions a fixed 22-px window at the bottom of the main viewport.
 * Left side: version · project name · active file · cursor position.
 * Right side: entity counts and error / warning counts.
 */
void StatusBarPanel::render()
{
    if (!m_isVisible)
        return;

    auto* t = getTranslationsManager();
    const float h = getHeight();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 pos  = {viewport->Pos.x, viewport->Pos.y + viewport->Size.y - h};
    ImVec2 size = {viewport->Size.x, h};

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar     | ImGuiWindowFlags_NoResize     |
        ImGuiWindowFlags_NoMove         | ImGuiWindowFlags_NoScrollbar  |
        ImGuiWindowFlags_NoSavedSettings| ImGuiWindowFlags_NoDocking    |
        ImGuiWindowFlags_NoNav;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, BG0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,   ImVec2(10.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("##statusbar", nullptr, flags);

    // Top-edge accent line
    {
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = {p0.x + ImGui::GetWindowWidth(), p0.y};
        ImGui::GetWindowDrawList()->AddLine(p0, p1,
            ImGui::ColorConvertFloat4ToU32(BORDER_HI), 1.0f);
    }

    // Align text vertically in the bar
    float textY = (h - ImGui::GetTextLineHeight()) * 0.5f;

    ImGui::SetCursorPosY(textY);

    // ----- Left side --------------------------------------------------------

    auto sep = [&]() {
        ImGui::SameLine(0, 8);
        ImGui::TextColored(TEXT2, "|");
        ImGui::SameLine(0, 8);
    };

    ImGui::TextColored(TEXT0, "%s", std::format("{}: {}", t->_t("STATUSBAR.VERSION"), m_version).c_str());
    sep();
    const std::string projectLabel =
        m_projectName.empty() ? t->_t("PROJECT.NO_PROJECT") : m_projectName;
    ImGui::TextColored(TEXT0, "%s", projectLabel.c_str());

    if (!m_activeFile.empty()) {
        sep();
        ImGui::TextColored(TEXT1, "%s", m_activeFile.c_str());
    }

    sep();
    {
        // auto* t = getTranslationsManager();
        std::string cursor = std::format("{} {}, {} {}",
            t->_t("STATUSBAR.LINE"), m_line, t->_t("STATUSBAR.COLUMN"), m_col);
        ImGui::TextColored(TEXT1, "%s", cursor.c_str());
    }

    // ----- Right side — measure first, then render at the correct X --------

    struct Seg { std::string text; ImVec4 color; };
    std::vector<Seg> right;

    // Scene / NPC counters intentionally omitted — they duplicated the
    // Project tree's own stats strip.
    if (m_warnings > 0 || m_errors > 0) {
        if (m_warnings > 0)
            right.push_back({std::format("{} {}", m_warnings, t->_t("STATUSBAR.WARNINGS")), C_WARN});
        if (m_warnings > 0 && m_errors > 0)
            right.push_back({"·", TEXT2});
        if (m_errors > 0)
            right.push_back({std::format("{} {}", m_errors, t->_t("STATUSBAR.ERRORS")), C_ERROR});
    }

    // Calculate total width with 6 px gaps between segments
    float totalW = 0.0f;
    for (std::size_t i = 0; i < right.size(); ++i) {
        totalW += ImGui::CalcTextSize(right[i].text.c_str()).x;
        if (i + 1 < right.size()) totalW += 6.0f;
    }

    if (!right.empty()) {
        float rightX = ImGui::GetWindowWidth() - totalW - 10.0f;
        ImGui::SameLine(rightX);
        ImGui::SetCursorPosY(textY);

        for (std::size_t i = 0; i < right.size(); ++i) {
            ImGui::TextColored(right[i].color, "%s", right[i].text.c_str());
            if (i + 1 < right.size()) ImGui::SameLine(0, 6);
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    ImGui::End();
}

} // namespace ADS::IDE::Panels
