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
 * @file PropertiesPanel.cpp
 * @brief Implementation of the PropertiesPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "PropertiesPanel.h"
#include "imgui.h"
#include <cstring>

namespace ADS::IDE::Panels {
    /**
     * @brief Construct a new PropertiesPanel object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the properties panel with name "Properties" and
     * sets default values for all property fields. The name is set to
     * "Object Name", description to "Object description...", property
     * value to 50, and both visibility and interactive flags to true.
     */
    PropertiesPanel::PropertiesPanel()
        : BasePanel("Properties"),
          m_propertyValue(50),
          m_isVisibleFlag(true),
          m_isInteractiveFlag(true) {
        // Initialize character arrays
        std::strcpy(m_name, "Object Name");
        std::strcpy(m_description, "Object description...");
    }

    /**
     * @brief Handle apply button click
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the user action when the "Apply" button is clicked.
     * This method will be responsible for applying the current property
     * changes to the selected object.
     *
     * @note Currently a placeholder implementation
     */
    void PropertiesPanel::handleApply() {
        // Handle apply
    }

    /**
     * @brief Handle reset button click
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the user action when the "Reset" button is clicked.
     * This method will be responsible for resetting properties to their
     * default or previous values.
     *
     * @note Currently a placeholder implementation
     */
    void PropertiesPanel::handleReset() {
        // Handle reset
    }

    /**
     * @brief Render the properties panel
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays property editors for name, description, value,
     * and various flags with apply and reset buttons. The panel
     * provides input controls for modifying object properties.
     *
     * @note Returns early if panel is not visible
     * @see handleApply(), handleReset()
     */
    void PropertiesPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(m_windowName.c_str());

        ImGui::Text("Properties Panel");
        ImGui::Separator();

        // Property editors
        ImGui::InputText("Name", m_name, IM_ARRAYSIZE(m_name));
        ImGui::InputTextMultiline("Description", m_description, IM_ARRAYSIZE(m_description), ImVec2(-1, 100));
        ImGui::SliderInt("Value", &m_propertyValue, 0, 100);
        ImGui::Checkbox("Visible", &m_isVisibleFlag);
        ImGui::Checkbox("Interactive", &m_isInteractiveFlag);

        ImGui::Separator();

        if (ImGui::Button("Apply")) {
            handleApply();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            handleReset();
        }

        ImGui::End();
    }
}
