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


#include "LayoutManager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>

#include "spdlog/spdlog.h"

namespace ADS::IDE {
    LayoutManager::LayoutManager(): IDEBase(),
        m_isDockingSetup(false), m_dockSpaceId(0) {
    }

    bool LayoutManager::hasSavedLayout() {
        ImGuiDockNode *existingNode = ImGui::DockBuilderGetNode(m_dockSpaceId);
        return existingNode != nullptr && existingNode->IsSplitNode();
    }

    void LayoutManager::createDefaultLayout() {
        spdlog::info("No saved layout found, creating default layout...");
        // std::cout << "No saved layout found, creating default layout..." << std::endl;

        // Clear any existing layout
        ImGui::DockBuilderRemoveNode(m_dockSpaceId);
        ImGui::DockBuilderAddNode(m_dockSpaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(m_dockSpaceId, ImGui::GetMainViewport()->Size);

        // Split the dockspace into left, center, and right
        ImGuiID dock_main_id = m_dockSpaceId;
        ImGuiID dock_left_id  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left,  0.20f, nullptr, &dock_main_id);
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);

        // Dock windows to their respective areas using the same translated titles the panels use
        auto* tm = getTranslationManager();
        ImGui::DockBuilderDockWindow(tm->_t("ENTITIES").c_str(),     dock_left_id);
        ImGui::DockBuilderDockWindow(tm->_t("INSPECTOR").c_str(),    dock_right_id);
        ImGui::DockBuilderDockWindow(tm->_t("WORKING_AREA").c_str(), dock_main_id);

        // Finalize the docking layout
        ImGui::DockBuilderFinish(m_dockSpaceId);
    }

    void LayoutManager::setupDockingLayout() {
        // Only setup once
        if (m_isDockingSetup) {
            return;
        }

        m_isDockingSetup = true;

        // Check if there's an existing layout
        if (hasSavedLayout()) {
            spdlog::info("Found saved layout, using it...");
            return;
        }

        // No saved layout found, create default layout
        createDefaultLayout();
    }

    void LayoutManager::resetLayout() {
        m_isDockingSetup = false;
    }

    void LayoutManager::setDockSpaceId(ImGuiID dockSpaceId) {
        m_dockSpaceId = dockSpaceId;
    }

    ImGuiID LayoutManager::getDockSpaceId() const {
        return m_dockSpaceId;
    }
}
