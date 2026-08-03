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

namespace ADS::IDE::Panels {

using namespace ADS::IDE::Colors;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

StatusBarPanel::StatusBarPanel()
    : BasePanel("StatusBar")
{}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

void StatusBarPanel::setProjectName(const std::string& name)   { m_projectName = name; }
void StatusBarPanel::setActiveFile(const std::string& filename) { m_activeFile  = filename; }
void StatusBarPanel::setCursorPosition(int line, int col)       { m_line = line; m_col = col; }

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

float StatusBarPanel::getHeight() const
{
    // Computed each call so DPI / font changes are reflected automatically.
    // WindowPadding.y (4 px) × 2 gives comfortable vertical breathing room.
    return ImGui::GetTextLineHeight() + ImGui::GetStyle().WindowPadding.y * 2.0f;
}

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------

void StatusBarPanel::render()
{
    if (!m_isVisible)
        return;

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

    ImGui::TextColored(TEXT0, "ADS 0.4.2");
    sep();
    ImGui::TextColored(TEXT0, "%s", m_projectName.c_str());

    if (!m_activeFile.empty()) {
        sep();
        ImGui::TextColored(TEXT1, "%s", m_activeFile.c_str());
    }

    sep();
    ImGui::TextColored(TEXT1, "Línea %d, Col %d", m_line, m_col);

    // ----- Right side — measure first, then render at the correct X --------

    struct Seg { std::string text; ImVec4 color; };
    std::vector<Seg> right;

    right.push_back({std::format("{} escenas", m_scenes), TEXT0});
    right.push_back({"|", TEXT2});
    right.push_back({std::format("{} NPC", m_npcs), TEXT0});

    if (m_warnings > 0 || m_errors > 0) {
        right.push_back({"|", TEXT2});
        if (m_warnings > 0)
            right.push_back({std::format("{} warns", m_warnings), C_WARN});
        if (m_warnings > 0 && m_errors > 0)
            right.push_back({"·", TEXT2});
        if (m_errors > 0)
            right.push_back({std::format("{} err", m_errors), C_ERROR});
    }

    // Calculate total width with 6 px gaps between segments
    float totalW = 0.0f;
    for (std::size_t i = 0; i < right.size(); ++i) {
        totalW += ImGui::CalcTextSize(right[i].text.c_str()).x;
        if (i + 1 < right.size()) totalW += 6.0f;
    }

    float rightX = ImGui::GetWindowWidth() - totalW - 10.0f;
    ImGui::SameLine(rightX);
    ImGui::SetCursorPosY(textY);

    for (std::size_t i = 0; i < right.size(); ++i) {
        ImGui::TextColored(right[i].color, "%s", right[i].text.c_str());
        if (i + 1 < right.size()) ImGui::SameLine(0, 6);
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    ImGui::End();
}

} // namespace ADS::IDE::Panels
