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


#include "WorkingAreaPanel.h"
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include <string>
#include <cstring>

namespace ADS::IDE::Panels {
    WorkingAreaPanel::WorkingAreaPanel()
        : BasePanel("Working Area") {
        // Initialize script text with default content
        std::strcpy(m_scriptText,
                    ICON_FA_TREE " Forest Entrance\n"
                    ICON_FA_BOOK " Description: You stand at the edge of a dark forest...\n"
                    ICON_FA_COMMENT " Dialog: 'Welcome, traveler...'\n");
    }

    void WorkingAreaPanel::renderTabBar() {
        if (ImGui::BeginTabBar("DocumentTabs")) {
            // Script 1 tab
            std::string tabLabel = std::string(ICON_FA_CARET_UP) + " Script 1";
            if (ImGui::BeginTabItem(tabLabel.c_str())) {
                renderScriptEditor();
                ImGui::EndTabItem();
            }

            // Script 2 tab
            if (ImGui::BeginTabItem("Script 2")) {
                ImGui::Text("Content of script 2 - %s", ICON_FA_CHECK);
                ImGui::EndTabItem();
            }

            // Add new tab button
            if (ImGui::BeginTabItem("+")) {
                ImGui::Text("Click to add new script");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    void WorkingAreaPanel::renderScriptEditor() {
        ImGui::InputTextMultiline("##script", m_scriptText, IM_ARRAYSIZE(m_scriptText), ImVec2(-1, -1));
    }

    void WorkingAreaPanel::handleNewTab() {
        // Handle new tab creation
    }

    void WorkingAreaPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(m_windowName.c_str());

        ImGui::Text("Main Content Area");
        ImGui::Separator();

        renderTabBar();

        ImGui::End();
    }
}
