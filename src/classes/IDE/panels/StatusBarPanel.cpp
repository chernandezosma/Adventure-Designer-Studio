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

/**
 * @file StatusBarPanel.cpp
 * @brief Implementation of the StatusBarPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "StatusBarPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Construct a new StatusBarPanel object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the status bar panel with name "Status Bar" and
     * sets initial height to 0.0f. The actual height is calculated
     * dynamically during rendering.
     */
    StatusBarPanel::StatusBarPanel()
        : BasePanel("Status Bar"), m_height(0.0f) {
    }

    /**
     * @brief Calculate the height of the status bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Calculates the height based on ImGui frame height and spacing.
     * This method should be called during rendering to update the
     * height based on current ImGui style settings.
     *
     * @note This method updates the m_height member variable
     */
    void StatusBarPanel::calculateHeight() {
        m_height = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;
    }

    /**
     * @brief Render the status bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the status bar at the bottom of the viewport with
     * status information, cursor position, and FPS counter. The bar
     * is positioned as a fixed window at the bottom with no title bar,
     * borders, or docking capabilities.
     *
     * @note Returns early if panel is not visible
     * @see calculateHeight()
     */
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

    /**
     * @brief Get the height of the status bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Returns the current calculated height of the status bar. This
     * value is used by other components to properly size and position
     * their content relative to the status bar.
     *
     * @return float Height in pixels
     * @note The height is calculated during each render() call
     */
    float StatusBarPanel::getHeight() const {
        return m_height;
    }
}
