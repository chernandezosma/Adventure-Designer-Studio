/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material
 */


#include "MenuBarRenderer.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include "../themes/DarkTheme.h"
#include "../themes/LightTheme.h"

namespace ADS::IDE {
    /**
     * @brief Construct a new MenuBarRenderer object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the menu bar renderer with references to required services.
     * The constructor sets up the layout manager reference, creates a new
     * NavigationService instance, and obtains the translation manager from
     * the IDEBase parent class for internationalization support.
     *
     * @param layoutManager Pointer to the layout manager for layout operations
     *
     * @note Locale management is handled by the IDEBase parent class
     * @see IDEBase::IDEBase()
     */
    MenuBarRenderer::MenuBarRenderer(LayoutManager *layoutManager) :
        IDEBase(),
        m_layoutManager(layoutManager),
        m_navigationService(std::make_unique<NavigationService>()),
        m_translationManager(this->getTranslationManager())
    {
        // Locale is now managed in IDEBase
    }

    /**
     * @brief Render the File menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the File menu containing file operations. Creates menu items for:
     * - New (Ctrl+N): Creates a new file via NavigationService
     * - Open (Ctrl+O): Opens an existing file via NavigationService
     * - Save (Ctrl+S): Saves the current file (placeholder implementation)
     * - Exit (Alt+F4): Closes the application via handleExit()
     *
     * All menu labels are retrieved from the translation manager for i18n support.
     *
     * @note Should be called within an active ImGui menu bar context
     * @see NavigationService::fileNewHandler()
     * @see NavigationService::fileOpenHandler()
     * @see handleExit()
     */
    void MenuBarRenderer::renderFileMenu()
    {
        if (ImGui::BeginMenu(m_translationManager->_t("MENU.FILE_HEADER").data())) {
            if (ImGui::MenuItem(m_translationManager->_t("MENU.FILE_NEW").data(), "Ctrl+N")) {
                this->m_navigationService->fileNewHandler();
            }
            if (ImGui::MenuItem(m_translationManager->_t("MENU.FILE_OPEN").data(), "Ctrl+O")) {
                this->m_navigationService->fileOpenHandler();
            }
            if (ImGui::MenuItem(m_translationManager->_t("MENU.FILE_SAVE").data(), "Ctrl+S")) {
                // Handle save
            }
            ImGui::Separator();
            if (ImGui::MenuItem(m_translationManager->_t("MENU.FILE_EXIT").data(), "Alt+F4")) {
                handleExit();
            }
            ImGui::EndMenu();
        }
    }

    /**
     * @brief Render the Edit menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the Edit menu containing standard editing operations. Creates menu items for:
     * - Undo (Ctrl+Z): Undo last action (placeholder implementation)
     * - Redo (Shift+Ctrl+Z): Redo previously undone action (placeholder implementation)
     * - Copy (Ctrl+C): Copy selection to clipboard (placeholder implementation)
     * - Cut (Ctrl+X): Cut selection to clipboard (placeholder implementation)
     * - Paste (Ctrl+V): Paste from clipboard (placeholder implementation)
     *
     * All menu labels are retrieved from the translation manager for i18n support.
     *
     * @note Should be called within an active ImGui menu bar context
     * @note All edit operations currently contain placeholder implementations
     */
    void MenuBarRenderer::renderEditMenu()
    {
        if (ImGui::BeginMenu(m_translationManager->_t("MENU.EDIT_HEADER").data())) {

            if (ImGui::MenuItem(m_translationManager->_t("MENU.EDIT_UNDO").data(), "Ctrl+Z")) {

            }

            if (ImGui::MenuItem(m_translationManager->_t("MENU.EDIT_REDO").data(), "Shift+Ctrl+Z")) {

            }
            ImGui::Separator();

            if (ImGui::MenuItem(m_translationManager->_t("MENU.EDIT_COPY").data(), "Ctrl+C")) {

            }
            if (ImGui::MenuItem(m_translationManager->_t("MENU.EDIT_CUT").data(), "Ctrl+X")) {

            }

            if (ImGui::MenuItem(m_translationManager->_t("MENU.EDIT_PASTE").data(), "Ctrl+V")) {

            }
            ImGui::EndMenu();
        }
    }

    /**
     * @brief Render the View menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the View menu containing view-related operations. Creates menu items for:
     * - Zoom In (Ctrl++): Increase view zoom level (placeholder implementation)
     * - Zoom Out (Ctrl+-): Decrease view zoom level (placeholder implementation)
     * - Reset Layout: Restores the default IDE layout via LayoutManager
     *
     * All menu labels are retrieved from the translation manager for i18n support.
     *
     * @note Should be called within an active ImGui menu bar context
     * @see LayoutManager::resetLayout()
     */
    void MenuBarRenderer::renderViewMenu()
    {
        if (ImGui::BeginMenu(m_translationManager->_t("MENU.VIEW_HEADER").data())) {
            if (ImGui::MenuItem(m_translationManager->_t("MENU.VIEW_ZOOM_IN").data(), "Ctrl++")) {

            }
            if (ImGui::MenuItem(m_translationManager->_t("MENU.VIEW_ZOOM_OUT").data(), "Ctrl+-")) {

            }

            ImGui::Separator();
            if (ImGui::MenuItem(m_translationManager->_t("MENU.VIEW_RESET_LAYOUT").data())) {
                m_layoutManager->resetLayout();
            }
            ImGui::EndMenu();
        }
    }

    /**
     * @brief Render the Options menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the Options menu containing application settings. Creates menu items for:
     * - Language Selector: Opens language selection dialog (placeholder implementation)
     * - Theme submenu:
     *   - Dark Theme: Applies dark color scheme via handleThemeChange()
     *   - Light Theme: Applies light color scheme via handleThemeChange()
     *
     * All menu labels are retrieved from the translation manager for i18n support.
     *
     * @note Should be called within an active ImGui menu bar context
     * @see handleThemeChange()
     */
    void MenuBarRenderer::renderOptionsMenu()
    {
        if (ImGui::BeginMenu(m_translationManager->_t("MENU.OPTIONS_HEADER").data())) {
            if (ImGui::MenuItem(m_translationManager->_t("MENU.OPTIONS_LANGUAGE_SELECTOR").data())) {

            }
            ImGui::Separator();
            ImGui::Separator();
            if (ImGui::BeginMenu(m_translationManager->_t("MENU.VIEW_THEME").data())) {
                if (ImGui::MenuItem(m_translationManager->_t("MENU.VIEW_DARK_THEME").data())) {
                    handleThemeChange(true);
                }
                if (ImGui::MenuItem(m_translationManager->_t("MENU.VIEW_LIGHT_THEME").data())) {
                    handleThemeChange(false);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

    }

    /**
     * @brief Render the Help menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the Help menu containing help-related items. Creates menu items for:
     * - About: Shows application information dialog (placeholder implementation)
     *
     * All menu labels are retrieved from the translation manager for i18n support.
     *
     * @note Should be called within an active ImGui menu bar context
     */
    void MenuBarRenderer::renderHelpMenu()
    {
        if (ImGui::BeginMenu(m_translationManager->_t("MENU.HELP_HEADER").data())) {
            if (ImGui::MenuItem(m_translationManager->_t("MENU.HELP_ABOUT").data())) {
                // Show about dialog
            }
            ImGui::EndMenu();
        }
    }

    /**
     * @brief Handle exit action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initiates application shutdown by creating and pushing an SDL_QUIT event
     * to the SDL event queue. This signals the main application loop to begin
     * its graceful shutdown sequence.
     *
     * The function creates an SDL_Event structure, sets its type to SDL_QUIT,
     * and pushes it to the event queue using SDL_PushEvent().
     *
     * @note This method only signals shutdown; it does not perform cleanup
     * @see SDL_PushEvent()
     */
    void MenuBarRenderer::handleExit()
    {
        SDL_Event quit_event = {};
        quit_event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit_event);
    }

    /**
     * @brief Handle theme change
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Applies the specified theme to the ImGui interface. Creates an instance of
     * either DarkTheme or LightTheme based on the parameter and calls its apply()
     * method to update the ImGui style colors.
     *
     * @param darkTheme True to apply dark theme, false to apply light theme
     *
     * @see DarkTheme::apply()
     * @see LightTheme::apply()
     */
    void MenuBarRenderer::handleThemeChange(bool darkTheme)
    {
        if (darkTheme) {
            DarkTheme theme;
            theme.apply();
        } else {
            LightTheme theme;
            theme.apply();
        }
    }

    /**
     * @brief Register project-awareness callbacks on the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Forwards the callbacks directly to the owned NavigationService so it can
     * detect an active project and trigger new-project creation without holding
     * a direct reference to IDERenderer.
     *
     * @param hasProject   Predicate returning true when a project is open
     * @param onNewProject Callable invoked to create a fresh project
     * @see NavigationService::setProjectCallbacks()
     */
    void MenuBarRenderer::setNavigationCallbacks(
        std::function<bool()> hasProject,
        std::function<void()> onNewProject)
    {
        m_navigationService->setProjectCallbacks(
            std::move(hasProject),
            std::move(onNewProject)
        );
    }

    /**
     * @brief Register file I/O callbacks forwarded to the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates directly to NavigationService::setFileCallbacks(). See that
     * method's documentation for the callback semantics.
     *
     * @param onOpen  Callable receiving the absolute path from the Open dialog
     * @param onSave  Callable receiving the absolute path from the Save dialog
     * @see NavigationService::setFileCallbacks()
     */
    void MenuBarRenderer::setFileCallbacks(
        std::function<void(const std::string&)> onOpen,
        std::function<void(const std::string&)> onSave)
    {
        m_navigationService->setFileCallbacks(
            std::move(onOpen),
            std::move(onSave)
        );
    }

    /**
     * @brief Render any pending modal dialogs from the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to NavigationService::renderDialogs(). Called by IDERenderer
     * once per frame after EndMenuBar but still within the MainDockSpace window,
     * ensuring the correct ImGui window context for popup rendering.
     *
     * @see NavigationService::renderDialogs()
     */
    void MenuBarRenderer::renderDialogs()
    {
        m_navigationService->renderDialogs();
    }

    /**
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to NavigationService::processPendingDialogs(). Called by
     * IDERenderer after SDL_RenderPresent so that blocking NFD calls do not
     * freeze the render loop mid-frame.
     *
     * @see NavigationService::processPendingDialogs()
     */
    void MenuBarRenderer::processPendingDialogs()
    {
        m_navigationService->processPendingDialogs();
    }

    /**
     * @brief Render the menu bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders the complete menu bar by sequentially calling all menu render methods.
     * This method orchestrates the rendering of the File, Edit, View, Options, and
     * Help menus in the correct order.
     *
     * @note Should be called within an active ImGui::BeginMenuBar() context
     * @see renderFileMenu()
     * @see renderEditMenu()
     * @see renderViewMenu()
     * @see renderOptionsMenu()
     * @see renderHelpMenu()
     */
    void MenuBarRenderer::render()
    {
        renderFileMenu();
        renderEditMenu();
        renderViewMenu();
        renderOptionsMenu();
        renderHelpMenu();
    }
}
