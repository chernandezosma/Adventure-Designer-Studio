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
#include "EntityKind.h"
#include "LayoutManager.h"
#include "navigation/MenuBarRenderer.h"
#include "navigation/ToolBarRenderer.h"
#include "panels/StatusBarPanel.h"
#include "panels/ProjectTreePanel.h"
#include "panels/InspectorPanel.h"
#include "panels/WorkingAreaPanel.h"
#include "panels/TranslationPanel.h"
#include "dialogs/NewProjectDialog.h"
#include "dialogs/LoadWarningsDialog.h"
#include "Core/Project.h"
#include <string>
#include <utility>
#include <vector>

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
         * Project tree panel on the left
         */
        Panels::ProjectTreePanel *m_projectTreePanel;

        /**
         * Inspector panel on the right
         */
        Panels::InspectorPanel *m_inspectorPanel;

        /**
         * Working area panel in the center
         */
        Panels::WorkingAreaPanel *m_workingAreaPanel;

        /**
         * Translation editor panel (View ▸ Translations). Floating / dockable,
         * hidden by default, rendered every frame.
         */
        Panels::TranslationPanel *m_translationPanel;

        /**
         * Modal collecting name/languages/author for a brand-new project.
         * Rendered every frame from renderMainWindow(); armed by the File > New
         * navigation callback.
         */
        NewProjectDialog *m_newProjectDialog;

        /**
         * Modal listing entities skipped during the most recent partial project
         * load. Rendered every frame from renderMainWindow(); armed by the
         * File > Open callback whenever ProjectSerializer::load() returns any
         * LoadWarning entries.
         */
        LoadWarningsDialog *m_loadWarningsDialog;

        /**
         * Owning pointer to the active project (created in initializePanels)
         */
        Core::Project *m_project;

        /**
         * True while the project has unsaved changes.
         * Set by onPropertyChanged / onAddNode / newProjectFromSpec callbacks.
         * Cleared after a successful save (future).
         */
        bool m_hasUnsavedChanges = false;

        /**
         * Message from the last failed save, shown in the project info bar
         * until the next save attempt. Empty when the last save succeeded or
         * none has been attempted — a save must never fail silently.
         */
        std::string m_lastSaveError;

        /**
         * Message from the last failed project open (whole-file failure —
         * missing file, malformed JSON, unsupported schema version, or a
         * checksum mismatch), shown in the project info bar until the next
         * open attempt. Empty when the last open succeeded, or partially
         * succeeded with only per-entity warnings (see m_loadWarningsDialog).
         */
        std::string m_lastOpenError;

        /**
         * @brief Create a new bare State, mirroring the tree's "Add > State"
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Auto-names it "Nuevo estado N" the same way onAddNode does, links
         * it to nothing, rebuilds the project tree, and marks unsaved
         * changes. Shared by onAddNode's State case and
         * InspectorPanel::onCreateState so both paths behave identically.
         *
         * @return Entities::State* The newly created state, or nullptr on failure
         */
        Entities::State* createNewState();

        /**
         * @brief Create a new bare StateChain, mirroring "Add > Chain"
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return Entities::StateChain* The newly created chain, or nullptr on failure
         */
        Entities::StateChain* createNewChain();

        /**
         * @brief Create a fresh entity of the given kind.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Auto-names it ("New scene 12", …) with a free id from
         * Core::Project::nextXId(), then refreshes the tree and status bar and
         * marks the project dirty. Shared by the tree's "+" / context menu and
         * the "Entities" menu.
         *
         * @param kind Entity type to create
         */
        void createEntity(EntityKind kind);

        /**
         * @brief Deep-copy an existing entity (new id + " (copy)" name).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Resolves @p id within the @p kind collection, calls the matching
         * Core::Project::duplicateX(), then selects the copy in the Inspector.
         * No-op when @p id is unknown.
         *
         * @param kind Entity type
         * @param id   String id of the entity to copy (BaseEntity::getId())
         */
        void duplicateEntity(EntityKind kind, const std::string& id);

        /**
         * @brief Delete the entity of @p kind with the given string id.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Clears the Inspector selection first when it points at that entity,
         * then calls the matching Core::Project::removeX(). No-op when @p id is
         * unknown.
         *
         * @param kind Entity type
         * @param id   String id of the entity to delete
         */
        void deleteEntity(EntityKind kind, const std::string& id);

        /**
         * @brief Current entities of @p kind as {stringId, displayName} pairs.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Feeds the "Entities" menu's Duplicate ▸ / Delete ▸ lists. Empty when
         * no project is open.
         *
         * @param kind Entity type
         * @return std::vector<std::pair<std::string, std::string>> id/name pairs
         */
        [[nodiscard]] std::vector<std::pair<std::string, std::string>>
        listEntities(EntityKind kind) const;

        /**
         * @brief Refresh tree + status bar + dirty flag after an entity change.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         */
        void refreshAfterEntityChange();

        /**
         * @brief Map a tree NodeType to its EntityKind.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param type Tree node type (must satisfy nodeTypeIsEntity())
         * @return EntityKind The matching kind (defaults to Scene)
         */
        static EntityKind nodeTypeToEntityKind(Panels::NodeType type);

        /**
         * @brief Whether @p type is one of the four full CRUD entity kinds.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param type Tree node type
         * @return bool true for Scene / NPC / Item / State
         */
        static bool nodeTypeIsEntity(Panels::NodeType type);

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
         * @brief Replace the active project with one built from a NewProjectSpec
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Builds a fresh Core::Project from @p spec (title, synopsis, author,
         * version, languages) and **writes its `.ads` file to
         * @c spec.projectPath**. Only if that write succeeds does it delete the
         * previous project, install the new one and refresh the tree / status
         * bar. On a write failure nothing changes — the caller (NewProjectDialog)
         * keeps its modal open and shows the returned message.
         *
         * @param spec Collected settings from the New Project modal
         * @return std::string Empty on success; the failure reason otherwise
         */
        std::string newProjectFromSpec(const NewProjectSpec& spec);

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
         * @brief Whether any native file dialog is currently running
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Aggregates MenuBarRenderer's NavigationService (Open/Save) and
         * InspectorPanel's Browse… dialog. NFD dialogs run on a background
         * thread and belong to a separate portal process with no
         * programmatic close, so App::run() checks this before honoring a
         * window-close request — quitting while this is true would leave the
         * dialog orphaned on screen.
         *
         * @return true if a background dialog thread is currently running
         */
        bool isDialogInProgress() const;

        /**
         * @brief Notify the IDE that the main window was resized
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Forwards to LayoutManager::onWindowResized() so the Project/Inspector/
         * Working Area panels are re-split at their default width ratios against
         * the new window size on the next frame.
         *
         * @see LayoutManager::onWindowResized()
         */
        void notifyWindowResized();

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
         * @brief Get the project tree panel.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return Panels::ProjectTreePanel* Pointer to the project tree panel instance.
         *
         * @note The returned pointer remains valid for the lifetime of the IDERenderer.
         */
        Panels::ProjectTreePanel *getProjectTreePanel() const;

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
