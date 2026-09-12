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
#include <algorithm>
#include <iostream>

#include "spdlog/spdlog.h"

namespace ADS::IDE {
    LayoutManager::LayoutManager(): IDEBase(),
        m_isDockingSetup(false), m_dockSpaceId(0), m_forceRebuild(false) {
    }

    /**
     * @brief Check if a saved layout exists
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Queries the ImGui docking system to determine if a previously saved layout
     * exists for the current dockspace. Checks if the dockspace node exists and
     * contains split information, which indicates a user-customized or saved layout.
     *
     * @return true if saved layout exists in imgui.ini and the node is split
     * @return false if no saved layout found or dockspace is uninitialized
     *
     * @note This method relies on ImGui's internal dock node structure
     * @see createDefaultLayout(), setupDockingLayout()
     */
    bool LayoutManager::hasSavedLayout() {
        ImGuiDockNode *existingNode = ImGui::DockBuilderGetNode(m_dockSpaceId);
        return existingNode != nullptr && existingNode->IsSplitNode();
    }

    /**
     * @brief Create the default docking layout
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Creates a split layout with predefined panel positions and sizes.
     * Removes any existing layout and builds a new docking configuration from scratch.
     * The default layout consists of:
     * - Left (20% width): Entities panel
     * - Center (remaining width): Working Area panel
     * - Right (25% width): Properties panel (top 50%) and Inspector panel (bottom 50%)
     *
     * Uses ImGui DockBuilder API to construct the layout programmatically and
     * finalizes it for immediate use.
     *
     * @note This is a private method called by setupDockingLayout()
     * @see setupDockingLayout(), hasSavedLayout()
     */
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

        // The 0.25 ratio alone gives the Inspector ~20% of the total width,
        // which clips its single-select rows (combo + inline "+" button) on a
        // small/HiDPI screen — and onWindowResized() re-runs this split on
        // every resize, so it keeps snapping back. Pin an explicit width that
        // is at least ~460 px (enough for the widest property label plus a
        // usable combo and the "+" button, even at 1.5x font scaling) and
        // otherwise scales with the viewport. InspectorPanel::
        // computeMinPanelWidth() then keeps the user from dragging it
        // narrower than its widest row needs.
        {
            const ImVec2 vp = ImGui::GetMainViewport()->Size;
            const float inspectorW = std::max(vp.x * 0.22f, 460.0f);
            ImGui::DockBuilderSetNodeSize(dock_right_id, ImVec2(inspectorW, vp.y));
        }

        // Dock windows by their stable IDs (see BasePanel::getImGuiLabel()) —
        // independent of the translated title text shown in each window's tab.
        ImGui::DockBuilderDockWindow("###Proyecto",     dock_left_id);
        ImGui::DockBuilderDockWindow("###hInspector",   dock_right_id);
        ImGui::DockBuilderDockWindow("###hWorkingArea", dock_main_id);

        // Finalize the docking layout
        ImGui::DockBuilderFinish(m_dockSpaceId);
    }

    /**
     * @brief Setup the docking layout
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Configures the ImGui docking layout for the IDE workspace. This method checks
     * if a layout has already been set up during the current session, and if so,
     * returns early to avoid redundant configuration.
     *
     * If no setup has occurred, it checks for a saved layout in imgui.ini using
     * hasSavedLayout(). If a saved layout exists, it is preserved and used. Otherwise,
     * createDefaultLayout() is called to establish the initial panel arrangement.
     *
     * This method should be called once per frame from the main dockspace window
     * rendering code, but will only perform actual setup on the first call unless
     * resetLayout() has been invoked.
     *
     * @note Only runs once per session unless resetLayout() is called
     * @see createDefaultLayout(), hasSavedLayout(), resetLayout()
     */
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

    /**
     * @brief Reset the layout setup flag
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Resets the internal docking setup flag to false, causing the layout to be
     * recreated on the next frame when setupDockingLayout() is called. This effectively
     * forces the creation of a fresh default layout, discarding any user customizations
     * or saved layout state.
     *
     * This method is typically invoked by the "Reset Layout" menu item in the View
     * menu, allowing users to restore the default panel arrangement if they've
     * accidentally misconfigured their workspace.
     *
     * @note The reset takes effect on the next call to setupDockingLayout()
     * @see setupDockingLayout(), createDefaultLayout()
     */
    void LayoutManager::resetLayout() {
        m_isDockingSetup = false;
        m_forceRebuild = true;
    }

    /**
     * @brief Notify the layout manager that the main window was resized
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Marks the default layout for an unconditional rebuild on the next
     * setupDockingLayout() call, so the Project/Inspector/Working Area
     * panels are re-split at their default width ratios against the new
     * window size. This overrides any manual panel-width adjustments the
     * user made by dragging a splitter.
     *
     * @note Actual DockBuilder calls happen later, inside
     *       setupDockingLayout(), since they must run within an ImGui frame.
     * @see setupDockingLayout(), createDefaultLayout()
     */
    void LayoutManager::onWindowResized() {
        m_forceRebuild = true;
    }

    /**
     * @brief Set the dockspace ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Assigns the ImGui dockspace identifier that this layout manager should
     * operate on. This ID is obtained from ImGui::GetID() in the main dockspace
     * window and must be set before calling setupDockingLayout().
     *
     * The dockspace ID serves as the root container for all docking operations
     * and layout configuration.
     *
     * @param dockSpaceId ImGui dockspace identifier obtained from ImGui::GetID()
     *
     * @note Must be called before setupDockingLayout() to ensure proper layout configuration
     * @see getDockSpaceId(), setupDockingLayout()
     */
    void LayoutManager::setDockSpaceId(ImGuiID dockSpaceId) {
        m_dockSpaceId = dockSpaceId;
    }

    /**
     * @brief Get the dockspace ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Returns the current ImGui dockspace identifier being managed by this
     * layout manager. This ID was previously set via setDockSpaceId() and
     * represents the root docking container.
     *
     * @return ImGuiID Current dockspace ID, or 0 if not yet set
     *
     * @see setDockSpaceId()
     */
    ImGuiID LayoutManager::getDockSpaceId() const {
        return m_dockSpaceId;
    }
}
