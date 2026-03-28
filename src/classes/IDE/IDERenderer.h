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


#ifndef ADS_IDE_RENDERER_H
#define ADS_IDE_RENDERER_H

#include "IDEBase.h"
#include "LayoutManager.h"
#include "navigation/MenuBarRenderer.h"
#include "navigation/ToolBarRenderer.h"
#include "panels/StatusBarPanel.h"
#include "panels/EntitiesPanel.h"
#include "panels/InspectorPanel.h"
#include "panels/WorkingAreaPanel.h"
#include "Core/Project.h"

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
         * Inspector panel on the right
         */
        Panels::InspectorPanel *m_inspectorPanel;

        /**
         * Working area panel in the center
         */
        Panels::WorkingAreaPanel *m_workingAreaPanel;

        /**
         * Owning pointer to the active project (created in initializePanels)
         */
        Core::Project *m_project;

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
         * @brief Create a fresh empty project, discarding the current one
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Replaces the active project with a new empty Core::Project instance.
         * Clears the inspector selection and updates the entities panel data
         * source so the UI reflects the empty state immediately.
         *
         * Called by the NavigationService callback registered in initializePanels()
         * when the user confirms "New project" via the File > New dialog.
         *
         * @note The old project is deleted; any unsaved data is lost
         * @see NavigationService::fileNewHandler()
         */
        void newProject();

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
         * @brief Execute any deferred native file dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Delegates to MenuBarRenderer::processPendingDialogs(). Must be called
         * by App::run() immediately after render() returns (i.e. after
         * SDL_RenderPresent) and before the next processEvents() call.
         *
         * This ordering guarantees that the compositor already holds a clean
         * rendered frame when the blocking NFD dialog is shown, preventing the
         * gray-window artifact caused by freezing the render loop mid-frame.
         *
         * @see MenuBarRenderer::processPendingDialogs()
         * @see App::run()
         */
        void processPendingDialogs();

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
