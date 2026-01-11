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
 * @file WorkingAreaPanel.cpp
 * @brief Implementation of the WorkingAreaPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "WorkingAreaPanel.h"
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include <string>
#include <cstring>

namespace ADS::IDE::Panels {
    /**
     * @brief Construct a new WorkingAreaPanel object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the working area panel with name "Working Area" and
     * sets up default script text with sample content including scene
     * description, dialog, and FontAwesome icons.
     */
    WorkingAreaPanel::WorkingAreaPanel()
        : BasePanel("Working Area") {
        // Initialize script text with default content
        std::strcpy(m_scriptText,
                    ICON_FA_TREE " Forest Entrance\n"
                    ICON_FA_BOOK " Description: You stand at the edge of a dark forest...\n"
                    ICON_FA_COMMENT " Dialog: 'Welcome, traveler...'\n");
    }

    /**
     * @brief Render the tab bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a tabbed interface for managing multiple open documents.
     * Shows tabs for each open script and provides a "+" tab for creating
     * new scripts. Each tab can contain different content.
     *
     * @note Currently displays placeholder tabs: Script 1, Script 2, and +
     * @see renderScriptEditor()
     */
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

    /**
     * @brief Render the script editor
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a multi-line text editor for editing script content.
     * The editor fills the available space in the tab and allows
     * free-form text input with FontAwesome icons support.
     *
     * @note Currently edits the m_scriptText buffer
     */
    void WorkingAreaPanel::renderScriptEditor() {
        ImGui::InputTextMultiline("##script", m_scriptText, IM_ARRAYSIZE(m_scriptText), ImVec2(-1, -1));
    }

    /**
     * @brief Handle new tab creation
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the user action when the "+" tab is clicked.
     * This method will be responsible for creating new script tabs
     * in the working area.
     *
     * @note Currently a placeholder implementation
     */
    void WorkingAreaPanel::handleNewTab() {
        // Handle new tab creation
    }

    /**
     * @brief Render the working area panel
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a tabbed interface for editing scripts with
     * multi-line text editors. The panel provides the main content
     * editing area with support for multiple open documents.
     *
     * @note Returns early if panel is not visible
     * @see renderTabBar(), renderScriptEditor()
     */
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
