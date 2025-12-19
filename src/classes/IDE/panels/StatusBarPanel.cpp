/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */


#include "StatusBarPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    StatusBarPanel::StatusBarPanel()
        : BasePanel("Status Bar"), m_height(0.0f) {
    }

    void StatusBarPanel::calculateHeight() {
        m_height = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
    }

    void StatusBarPanel::render() {
        if (!m_isVisible) {
            return;
        }

        // Calculate height for this frame
        calculateHeight();

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Position status bar at bottom of viewport
        ImVec2 statusBarPos = ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - m_height);
        ImVec2 statusBarSize = ImVec2(viewport->Size.x, m_height);

        ImGui::SetNextWindowPos(statusBarPos);
        ImGui::SetNextWindowSize(statusBarSize);

        ImGuiWindowFlags statusBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
        statusBarFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
        statusBarFlags |= ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 3.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin(m_windowName.c_str(), nullptr, statusBarFlags);
        ImGui::PopStyleVar(2);

        ImGui::Text("Ready | Line: 1, Col: 1 | FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::SameLine(ImGui::GetWindowWidth() - 150);
        ImGui::Text("Text Adventure IDE");

        ImGui::End();
    }

    float StatusBarPanel::getHeight() const {
        return m_height;
    }
}
