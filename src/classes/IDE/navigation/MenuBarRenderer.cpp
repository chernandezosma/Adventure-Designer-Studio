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
#include <SDL.h>
#include "../themes/DarkTheme.h"
#include "../themes/LightTheme.h"

namespace ADS::IDE {
    MenuBarRenderer::MenuBarRenderer(LayoutManager *layoutManager) :
        IDEBase(),
        m_layoutManager(layoutManager),
        m_navigationService()

    {
        // Locale is now managed in IDEBase
    }

    void MenuBarRenderer::renderFileMenu()
    {
        i18n::i18n* tm = this->getTranslationManager();

        if (ImGui::BeginMenu(tm->_t("MENU.FILE_HEADER").data())) {
            if (ImGui::MenuItem(tm->_t("MENU.FILE_NEW").data(), "Ctrl+N")) {
                // Handle new
            }
            if (ImGui::MenuItem(tm->_t("MENU.FILE_OPEN").data(), "Ctrl+O")) {
                // Handle open
            }
            if (ImGui::MenuItem(tm->_t("MENU.FILE_SAVE").data(), "Ctrl+S")) {
                // Handle save
            }
            ImGui::Separator();
            if (ImGui::MenuItem(tm->_t("MENU.FILE_EXIT").data(), "Alt+F4")) {
                handleExit();
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::renderEditMenu()
    {
        i18n::i18n* tm = this->getTranslationManager();

        if (ImGui::BeginMenu(tm->_t("MENU.EDIT_HEADER").data())) {

            if (ImGui::MenuItem(tm->_t("MENU.EDIT_UNDO").data(), "Ctrl+Z")) {

            }

            if (ImGui::MenuItem(tm->_t("MENU.EDIT_REDO").data(), "Shift+Ctrl+Z")) {

            }
            ImGui::Separator();

            if (ImGui::MenuItem(tm->_t("MENU.EDIT_COPY").data(), "Ctrl+C")) {

            }
            if (ImGui::MenuItem(tm->_t("MENU.EDIT_CUT").data(), "Ctrl+X")) {

            }

            if (ImGui::MenuItem(tm->_t("MENU.EDIT_PASTE").data(), "Ctrl+V")) {

            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::renderViewMenu()
    {
        i18n::i18n* tm = this->getTranslationManager();

        if (ImGui::BeginMenu(tm->_t("MENU.VIEW_HEADER").data())) {
            if (ImGui::MenuItem(tm->_t("MENU.VIEW_ZOOM_IN").data(), "Ctrl++")) {

            }
            if (ImGui::MenuItem(tm->_t("MENU.VIEW_ZOOM_OUT").data(), "Ctrl+-")) {

            }

            ImGui::Separator();
            if (ImGui::MenuItem(tm->_t("MENU.VIEW_RESET_LAYOUT").data())) {
                m_layoutManager->resetLayout();
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::renderOptionsMenu()
    {
        i18n::i18n* tm = this->getTranslationManager();

        if (ImGui::BeginMenu(tm->_t("MENU.OPTIONS_HEADER").data())) {
            if (ImGui::MenuItem(tm->_t("MENU.OPTIONS_LANGUAGE_SELECTOR").data())) {

            }
            ImGui::Separator();
            ImGui::Separator();
            if (ImGui::BeginMenu(tm->_t("MENU.VIEW_THEME").data())) {
                if (ImGui::MenuItem(tm->_t("MENU.VIEW_DARK_THEME").data())) {
                    handleThemeChange(true);
                }
                if (ImGui::MenuItem(tm->_t("MENU.VIEW_LIGHT_THEME").data())) {
                    handleThemeChange(false);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

    }

    void MenuBarRenderer::renderHelpMenu()
    {
        i18n::i18n* tm = this->getTranslationManager();

        if (ImGui::BeginMenu(tm->_t("MENU.HELP_HEADER").data())) {
            if (ImGui::MenuItem(tm->_t("MENU.HELP_ABOUT").data())) {
                // Show about dialog
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::handleExit()
    {
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    }

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

    void MenuBarRenderer::render()
    {
        renderFileMenu();
        renderEditMenu();
        renderViewMenu();
        renderOptionsMenu();
        renderHelpMenu();
    }
}
