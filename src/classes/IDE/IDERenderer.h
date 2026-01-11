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


#ifndef ADS_IDE_RENDERER_H
#define ADS_IDE_RENDERER_H

#include "IDEBase.h"
#include "LayoutManager.h"
#include "navigation/MenuBarRenderer.h"
#include "navigation/ToolBarRenderer.h"
#include "env/env.h"
#include "i18n/i18n.h"
#include "panels/StatusBarPanel.h"
#include "panels/EntitiesPanel.h"
#include "panels/PropertiesPanel.h"
#include "panels/InspectorPanel.h"
#include "panels/WorkingAreaPanel.h"

namespace ADS::IDE {
    /**
     * @brief Main orchestrator for IDE rendering
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Coordinates the rendering of all IDE components including panels,
     * layout management, and menu bar. Replaces the RenderIDE() function
     * from main.cpp.
     */
    class IDERenderer : public IDEBase
    {
    private:
        /**
         * Layout manager for docking configuration
         */
        LayoutManager *m_layoutManager;

        /**
         * Menu bar renderer
         */
        MenuBarRenderer *m_menuBarRenderer;

        /**
         * Toolbar renderer
         */
        ToolBarRenderer *m_toolBarRenderer;

        /**
         * Status bar panel at the bottom
         */
        Panels::StatusBarPanel *m_statusBarPanel;

        /**
         * Entities panel on the left
         */
        Panels::EntitiesPanel *m_entitiesPanel;

        /**
         * Properties panel on the right-top
         */
        Panels::PropertiesPanel *m_propertiesPanel;

        /**
         * Inspector panel on the right-bottom
         */
        Panels::InspectorPanel *m_inspectorPanel;

        /**
         * Working area panel in the center
         */
        Panels::WorkingAreaPanel *m_workingAreaPanel;

        /**
         * @brief Initialize all panels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Creates and initializes instances of all panel objects including the
         * layout manager, menu bar renderer, toolbar renderer, and all IDE panels
         * (status bar, entities, properties, inspector, and working area).
         * This method is called automatically during construction.
         *
         * @note This is a private method called by the constructor
         * @see IDERenderer()
         */
        void initializePanels();

        /**
         * @brief Render the main dockspace window
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders the main dockspace window that serves as the container for all
         * IDE panels. Calculates viewport dimensions accounting for status bar height,
         * configures window flags for proper docking behavior, creates the ImGui
         * dockspace, sets up the docking layout, and renders the menu bar.
         *
         * The window is positioned to fill the entire viewport except for the status
         * bar at the bottom, and uses passthrough central node flags to allow the
         * dockspace background to be transparent.
         *
         * @note This is a private method called by render()
         * @see render(), LayoutManager::setupDockingLayout()
         */
        void renderMainWindow();

    public:
        /**
         * @brief Construct a new IDERenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the IDE renderer by calling the IDEBase constructor to set up
         * shared infrastructure, then creates all panel objects, layout manager, and
         * navigation renderers through the initializePanels() method. All member
         * pointers are initialized to nullptr before panel creation.
         *
         * @note Automatically calls initializePanels() during construction
         * @see initializePanels(), IDEBase::IDEBase()
         */
        IDERenderer();

        /**
         * @brief Destroy the IDERenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Performs cleanup by deallocating all dynamically allocated panels and
         * components in the proper order. Deletes the status bar panel, entities panel,
         * properties panel, inspector panel, working area panel, toolbar renderer,
         * menu bar renderer, and layout manager.
         *
         * @note Deletion order is important to avoid accessing freed memory
         */
        ~IDERenderer();

        /**
         * @brief Render the complete IDE
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Main rendering method that orchestrates the rendering of all IDE components
         * in the correct order. Renders the main dockspace window with menu bar first,
         * followed by the toolbar, status bar at the bottom, and finally all dockable
         * panels (entities, properties, inspector, and working area).
         *
         * This method should be called once per frame from the main application render
         * loop. It replaces the monolithic RenderIDE() function from main.cpp with a
         * more modular and maintainable object-oriented approach.
         *
         * @note Must be called within an ImGui frame context (after ImGui_ImplSDL2_NewFrame)
         * @see renderMainWindow(), MenuBarRenderer::render(), ToolBarRenderer::render()
         */
        void render();

        /**
         * @brief Get the status bar panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Provides access to the status bar panel instance for reading status
         * information or updating status messages from external components.
         *
         * @return Panels::StatusBarPanel* Pointer to the status bar panel instance
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer
         */
        Panels::StatusBarPanel *getStatusBar() const;

        /**
         * @brief Get the entities panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Provides access to the entities panel instance for managing and displaying
         * the hierarchy of game entities and scene objects.
         *
         * @return Panels::EntitiesPanel* Pointer to the entities panel instance
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer
         */
        Panels::EntitiesPanel *getEntitiesPanel() const;

        /**
         * @brief Get the properties panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Provides access to the properties panel instance for displaying and editing
         * properties of the currently selected entity or object.
         *
         * @return Panels::PropertiesPanel* Pointer to the properties panel instance
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer
         */
        Panels::PropertiesPanel *getPropertiesPanel() const;

        /**
         * @brief Get the inspector panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Provides access to the inspector panel instance for displaying detailed
         * information and advanced properties of selected entities or resources.
         *
         * @return Panels::InspectorPanel* Pointer to the inspector panel instance
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer
         */
        Panels::InspectorPanel *getInspectorPanel() const;

        /**
         * @brief Get the working area panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Provides access to the working area panel instance which serves as the
         * main content area for scene editing, code editing, or other primary
         * work activities.
         *
         * @return Panels::WorkingAreaPanel* Pointer to the working area panel instance
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer
         */
        Panels::WorkingAreaPanel *getWorkingAreaPanel() const;
    };
}

#endif //ADS_IDE_RENDERER_H
