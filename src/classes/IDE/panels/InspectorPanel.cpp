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


#include "InspectorPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    InspectorPanel::InspectorPanel()
        : BasePanel("Inspector") {
    }

    void InspectorPanel::renderSelectionInfo() {
        ImGui::Text("Selected: None");
        ImGui::Text("Type: -");
        ImGui::Text("Location: -");
    }

    void InspectorPanel::renderMetadata() {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select an entity to view details");
    }

    void InspectorPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(m_windowName.c_str());

        ImGui::Text("Inspector Panel");
        ImGui::Separator();

        ImGui::TextWrapped("Details about the selected item will appear here.");
        ImGui::Spacing();

        renderSelectionInfo();

        ImGui::Separator();

        renderMetadata();

        ImGui::End();
    }
}
