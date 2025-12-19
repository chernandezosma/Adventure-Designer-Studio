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


#ifndef ADS_LAYOUT_MANAGER_H
#define ADS_LAYOUT_MANAGER_H

#include "imgui.h"

namespace ADS::IDE {
    /**
     * @brief Manages docking layout configuration for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Handles the setup and management of the ImGui docking layout.
     * Creates default layout on first run and provides functionality
     * to reset the layout when requested.
     */
    class LayoutManager {
    private:
        /**
         * Flag indicating if docking layout has been set up
         */
        bool m_isDockingSetup;

        /**
         * ImGui dockspace ID
         */
        ImGuiID m_dockSpaceId;

        /**
         * @brief Create the default docking layout
         *
         * Creates a split layout with:
         * - Left (20%): Entities panel
         * - Center: Working Area
         * - Right (20%): Properties (top 50%) and Inspector (bottom 50%)
         */
        void createDefaultLayout();

        /**
         * @brief Check if a saved layout exists
         *
         * @return true if saved layout exists in imgui.ini
         * @return false if no saved layout found
         */
        bool hasSavedLayout();

    public:
        /**
         * @brief Construct a new LayoutManager object
         */
        LayoutManager();

        /**
         * @brief Destroy the LayoutManager object
         */
        ~LayoutManager() = default;

        /**
         * @brief Setup the docking layout
         *
         * Configures the default layout if no saved layout exists.
         * If a saved layout exists in imgui.ini, it is preserved.
         * Only runs once per session unless reset is called.
         */
        void setupDockingLayout();

        /**
         * @brief Reset the layout setup flag
         *
         * Causes the layout to be recreated on the next frame.
         * Used by the "Reset Layout" menu item.
         */
        void resetLayout();

        /**
         * @brief Set the dockspace ID
         *
         * @param dockSpaceId ImGui dockspace identifier
         */
        void setDockSpaceId(ImGuiID dockSpaceId);

        /**
         * @brief Get the dockspace ID
         *
         * @return ImGuiID Current dockspace ID
         */
        ImGuiID getDockSpaceId() const;
    };
}

#endif //ADS_LAYOUT_MANAGER_H
