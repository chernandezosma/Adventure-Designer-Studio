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


#ifndef ADS_MENU_BAR_RENDERER_H
#define ADS_MENU_BAR_RENDERER_H

#include "NavigationService.h"
#include "../LayoutManager.h"
#include "i18n/i18n.h"
#include <functional>
#include <memory>

namespace ADS::IDE {
    /**
     * @brief Renders the main menu bar for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides menu functionality for File, Edit, View, Options, and Help menus
     * including theme switching, layout management, and language selection.
     * Integrates with NavigationService for handling menu actions.
     */
    class MenuBarRenderer: public IDEBase {
    private:
        /**
         * @brief Reference to the layout manager
         *
         * Provides access to layout management functionality for operations
         * like resetting the IDE layout to default settings.
         */
        LayoutManager* m_layoutManager;

        /**
         * @brief Service for handling navigation actions
         *
         * Unique pointer to NavigationService that handles file operations
         * and other navigation-related actions triggered from menu items.
         */
        std::unique_ptr<NavigationService> m_navigationService;

        /**
         * @brief Translation manager for internationalization
         *
         * Provides access to localized strings for menu items and labels,
         * enabling multi-language support in the menu bar.
         */
        i18n::i18n* m_translationManager;

        /**
         * @brief Render the File menu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the File menu containing file operations including New (Ctrl+N),
         * Open (Ctrl+O), Save (Ctrl+S), and Exit (Alt+F4). Delegates file operations
         * to the NavigationService.
         *
         * @note Should be called within an active ImGui menu bar context
         * @see NavigationService::fileNewHandler()
         * @see NavigationService::fileOpenHandler()
         */
        void renderFileMenu();

        /**
         * @brief Render the Edit menu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the Edit menu containing standard editing operations including
         * Undo (Ctrl+Z), Redo (Shift+Ctrl+Z), Copy (Ctrl+C), Cut (Ctrl+X), and
         * Paste (Ctrl+V). Currently contains placeholder implementations.
         *
         * @note Should be called within an active ImGui menu bar context
         */
        void renderEditMenu();

        /**
         * @brief Render the View menu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the View menu containing view-related operations including
         * Zoom In (Ctrl++), Zoom Out (Ctrl+-), and Reset Layout. Delegates
         * layout operations to the LayoutManager.
         *
         * @note Should be called within an active ImGui menu bar context
         * @see LayoutManager::resetLayout()
         */
        void renderViewMenu();

        /**
         * @brief Render the Options menu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the Options menu containing application settings including
         * Language Selector and Theme submenu (Dark Theme, Light Theme).
         * Theme changes are handled through handleThemeChange().
         *
         * @note Should be called within an active ImGui menu bar context
         * @see handleThemeChange()
         */
        void renderOptionsMenu();

        /**
         * @brief Render the Help menu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the Help menu containing help-related items including
         * the About dialog. Currently contains placeholder implementation
         * for the About dialog.
         *
         * @note Should be called within an active ImGui menu bar context
         */
        void renderHelpMenu();

        /**
         * @brief Handle exit action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initiates application shutdown by pushing an SDL_QUIT event to
         * the SDL event queue. This triggers the main application loop
         * to exit gracefully.
         *
         * @note This does not perform any cleanup; it only signals the
         *       application to begin its shutdown sequence
         */
        void handleExit();

        /**
         * @brief Handle theme change
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Applies the specified theme to the ImGui interface by creating
         * and applying either a DarkTheme or LightTheme instance based on
         * the parameter value.
         *
         * @param darkTheme True to apply dark theme, false to apply light theme
         *
         * @see DarkTheme::apply()
         * @see LightTheme::apply()
         */
        void handleThemeChange(bool darkTheme);

    public:
        /**
         * @brief Construct a new MenuBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the menu bar renderer with references to required services.
         * Sets up the layout manager, creates the navigation service, and obtains
         * the translation manager from the IDEBase parent class.
         *
         * @param layoutManager Pointer to the layout manager for layout operations
         *
         * @note The translation manager is obtained through IDEBase inheritance
         * @see IDEBase::getTranslationManager()
         */
        explicit MenuBarRenderer(LayoutManager* layoutManager);

        /**
         * @brief Destroy the MenuBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. The NavigationService is automatically cleaned up
         * through the unique_ptr.
         */
        ~MenuBarRenderer() = default;

        /**
         * @brief Register project-awareness callbacks on the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Forwards the two callbacks to the owned NavigationService so it can
         * query whether a project is active and trigger new-project creation.
         * Must be called after construction and before the first File > New action.
         *
         * @param hasProject   Predicate returning true when a project is open
         * @param onNewProject Callable invoked to create a fresh project
         * @see NavigationService::setProjectCallbacks()
         */
        void setNavigationCallbacks(
            std::function<bool()> hasProject,
            std::function<void()> onNewProject
        );

        /**
         * @brief Register file I/O callbacks forwarded to the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Forwards the open and save path callbacks to the owned NavigationService
         * so that native file dialog results reach the application layer. Must be
         * called before the user can trigger File > Open or the Save action in the
         * "New project" confirmation dialog.
         *
         * @param onOpen  Callable receiving the absolute path from the Open dialog
         * @param onSave  Callable receiving the absolute path from the Save dialog
         * @see NavigationService::setFileCallbacks()
         */
        void setFileCallbacks(
            std::function<void(const std::string&)> onOpen,
            std::function<void(const std::string&)> onSave
        );

        /**
         * @brief Render any pending modal dialogs from the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Delegates to NavigationService::renderDialogs(). Must be called once
         * per frame from within an active ImGui window, outside any
         * BeginMenu / EndMenu scope (typically after EndMenuBar).
         *
         * @see NavigationService::renderDialogs()
         */
        void renderDialogs();

        /**
         * @brief Execute any deferred native file dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Delegates to NavigationService::processPendingDialogs(). Must be
         * called after SDL_RenderPresent and before the next ImGui::NewFrame()
         * to ensure the compositor has a clean frame while the blocking NFD
         * call is in progress.
         *
         * @see NavigationService::processPendingDialogs()
         */
        void processPendingDialogs();

        /**
         * @brief Render the menu bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders the complete menu bar with all menus (File, Edit, View, Options,
         * and Help). This method orchestrates the rendering of all menu components
         * by calling individual render methods for each menu section.
         *
         * @note Should be called within an active ImGui::BeginMenuBar() context
         * @see renderFileMenu()
         * @see renderEditMenu()
         * @see renderViewMenu()
         * @see renderOptionsMenu()
         * @see renderHelpMenu()
         */
        void render();
    };
}

#endif //ADS_MENU_BAR_RENDERER_H
