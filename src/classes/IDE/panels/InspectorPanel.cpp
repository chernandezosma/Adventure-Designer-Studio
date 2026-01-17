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
 * @file InspectorPanel.cpp
 * @brief Implementation of the InspectorPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "InspectorPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Construct a new InspectorPanel object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the inspector panel with name "Inspector".
     */
    InspectorPanel::InspectorPanel()
        : BasePanel("Inspector") {
    }

    /**
     * @brief Render selection information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays basic information about the currently selected entity
     * including its name, type, and location. Shows "None" if no
     * entity is currently selected.
     *
     * @note Currently displays placeholder data
     */
    void InspectorPanel::renderSelectionInfo() {
        ImGui::Text("Selected: None");
        ImGui::Text("Type: -");
        ImGui::Text("Location: -");
    }

    /**
     * @brief Render metadata information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays additional metadata and detailed information about
     * the selected entity. Shows a message prompting the user to
     * select an entity if none is selected.
     *
     * @note Currently displays placeholder data
     */
    void InspectorPanel::renderMetadata() {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select an entity to view details");
    }

    /**
     * @brief Render the inspector panel
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays detailed information about the selected entity,
     * including selection details and metadata. The panel provides
     * an overview of the currently focused entity in the IDE.
     *
     * @note Returns early if panel is not visible
     * @see renderSelectionInfo(), renderMetadata()
     */
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
