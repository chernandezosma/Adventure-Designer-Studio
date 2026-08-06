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


#include "LayoutManager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>

#include "spdlog/spdlog.h"

namespace ADS::IDE {
    LayoutManager::LayoutManager(): IDEBase(),
        m_isDockingSetup(false), m_dockSpaceId(0), m_forceRebuild(false) {
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

        // Split the dockspace into left, center, and right.
        // NOTE: DockBuilderSplitNode's ratio is relative to the *remaining* area at
        // each step, not the total width. To make both side panels exactly 20% of
        // the TOTAL screen width:
        //   left  = 0.20                -> 20% of the full width
        //   right = target / (1 - left) -> 0.20 / 0.80 = 0.25, applied to the
        //                                  remaining 80%, so right = 0.25 * 0.80
        //                                  = 20% of the full width too
        ImGuiID dock_main_id = m_dockSpaceId;
        ImGuiID dock_left_id  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left,  0.20f, nullptr, &dock_main_id);
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);

        // Dock windows by their stable IDs (see BasePanel::getImGuiLabel()) —
        // independent of the translated title text shown in each window's tab.
        ImGui::DockBuilderDockWindow("###Proyecto",     dock_left_id);
        ImGui::DockBuilderDockWindow("###hInspector",   dock_right_id);
        ImGui::DockBuilderDockWindow("###hWorkingArea", dock_main_id);

        // Finalize the docking layout
        ImGui::DockBuilderFinish(m_dockSpaceId);
    }

    void LayoutManager::setupDockingLayout() {
        // Forced rebuild (window resize, or an explicit reset) bypasses
        // hasSavedLayout() entirely and always re-splits at the default ratios.
        if (m_forceRebuild) {
            m_forceRebuild = false;
            createDefaultLayout();
            return;
        }

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
        m_forceRebuild = true;
    }

    void LayoutManager::onWindowResized() {
        m_forceRebuild = true;
    }

    void LayoutManager::setDockSpaceId(ImGuiID dockSpaceId) {
        m_dockSpaceId = dockSpaceId;
    }

    ImGuiID LayoutManager::getDockSpaceId() const {
        return m_dockSpaceId;
    }
}
