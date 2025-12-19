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


#include "PropertiesPanel.h"
#include "imgui.h"
#include <cstring>

namespace ADS::IDE::Panels {
    PropertiesPanel::PropertiesPanel()
        : BasePanel("Properties"),
          m_propertyValue(50),
          m_isVisibleFlag(true),
          m_isInteractiveFlag(true) {
        // Initialize character arrays
        std::strcpy(m_name, "Object Name");
        std::strcpy(m_description, "Object description...");
    }

    void PropertiesPanel::handleApply() {
        // Handle apply
    }

    void PropertiesPanel::handleReset() {
        // Handle reset
    }

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
