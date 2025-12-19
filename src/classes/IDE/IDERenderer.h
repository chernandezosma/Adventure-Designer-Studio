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

#include "LayoutManager.h"
#include "MenuBarRenderer.h"
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
    class IDERenderer {
    private:
        /**
         * Layout manager for docking configuration
         */
        LayoutManager* m_layoutManager;

        /**
         * Menu bar renderer
         */
        MenuBarRenderer* m_menuBarRenderer;

        /**
         * Status bar panel at the bottom
         */
        Panels::StatusBarPanel* m_statusBarPanel;

        /**
         * Entities panel on the left
         */
        Panels::EntitiesPanel* m_entitiesPanel;

        /**
         * Properties panel on the right-top
         */
        Panels::PropertiesPanel* m_propertiesPanel;

        /**
         * Inspector panel on the right-bottom
         */
        Panels::InspectorPanel* m_inspectorPanel;

        /**
         * Working area panel in the center
         */
        Panels::WorkingAreaPanel* m_workingAreaPanel;

        /**
         * @brief Initialize all panels
         *
         * Creates instances of all panel objects.
         */
        void initializePanels();

        /**
         * @brief Render the main dockspace window
         *
         * Renders the main window with dockspace, calculates
         * the status bar height, and renders the menu bar.
         */
        void renderMainWindow();

    public:
        /**
         * @brief Construct a new IDERenderer object
         *
         * Initializes all panels, layout manager, and menu bar renderer.
         */
        IDERenderer();

        /**
         * @brief Destroy the IDERenderer object
         *
         * Cleans up all allocated panels and components.
         */
        ~IDERenderer();

        /**
         * @brief Render the complete IDE
         *
         * Main rendering method that orchestrates rendering of all IDE
         * components. Replaces the RenderIDE() function from main.cpp.
         */
        void render();

        /**
         * @brief Get the status bar panel
         *
         * @return Panels::StatusBarPanel* Pointer to status bar panel
         */
        Panels::StatusBarPanel* getStatusBar() const;

        /**
         * @brief Get the entities panel
         *
         * @return Panels::EntitiesPanel* Pointer to entities panel
         */
        Panels::EntitiesPanel* getEntitiesPanel() const;

        /**
         * @brief Get the properties panel
         *
         * @return Panels::PropertiesPanel* Pointer to properties panel
         */
        Panels::PropertiesPanel* getPropertiesPanel() const;

        /**
         * @brief Get the inspector panel
         *
         * @return Panels::InspectorPanel* Pointer to inspector panel
         */
        Panels::InspectorPanel* getInspectorPanel() const;

        /**
         * @brief Get the working area panel
         *
         * @return Panels::WorkingAreaPanel* Pointer to working area panel
         */
        Panels::WorkingAreaPanel* getWorkingAreaPanel() const;
    };
}

#endif //ADS_IDE_RENDERER_H
