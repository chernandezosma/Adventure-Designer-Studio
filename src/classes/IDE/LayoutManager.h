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

#include "IDEBase.h"
#include "imgui.h"
#include "i18n/i18n.h"

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
    class LayoutManager: public IDEBase{
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
        void createDefaultLayout();

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
        bool hasSavedLayout();

    public:
        /**
         * @brief Construct a new LayoutManager object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the layout manager by calling the IDEBase constructor to set up
         * shared infrastructure (translations and environment). Sets the docking setup
         * flag to false and initializes the dockspace ID to 0, indicating that layout
         * configuration has not yet been performed.
         *
         * @see IDEBase::IDEBase(), setupDockingLayout()
         */
        LayoutManager();

        /**
         * @brief Construct a new LayoutManager object with translations manager and environment
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Alternative constructor that accepts explicit references to the translations
         * manager and environment configuration. This constructor is currently declared
         * but not implemented, and may be deprecated in favor of the default constructor
         * which uses the IDEBase automatic initialization.
         *
         * @param translationsManager Pointer to the i18n instance for localization
         * @param environment Pointer to the Environment instance for configuration
         *
         * @note This constructor signature exists but may not be implemented
         * @see LayoutManager()
         */
        LayoutManager(i18n::i18n* translationsManager, Environment *environment);

        /**
         * @brief Destroy the LayoutManager object
         */
        ~LayoutManager() = default;

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
        void setupDockingLayout();

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
        void resetLayout();

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
        void setDockSpaceId(ImGuiID dockSpaceId);

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
        ImGuiID getDockSpaceId() const;
    };
}

#endif //ADS_LAYOUT_MANAGER_H
