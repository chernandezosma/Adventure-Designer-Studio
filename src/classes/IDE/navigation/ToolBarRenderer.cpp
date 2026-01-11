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


#include "ToolBarRenderer.h"
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include "../themes/DarkTheme.h"
#include "../themes/LightTheme.h"

namespace ADS::IDE {
    ToolBarRenderer::ToolBarRenderer(LayoutManager *layoutManager) :
        m_layoutManager(layoutManager),
        m_translationManager(this->getTranslationManager()),
        m_buttonHeight(26.0f),
        m_buttonPadding(4.0f)
    {
        // Locale is now managed in IDEBase
    }

    bool ToolBarRenderer::renderIconButton(const char *icon, const char *tooltip)
    {
        // Get the icon font from the font manager (inherited from IDEBase)
        UI::Fonts* fontManager = this->getFontManager();
        ImFont* iconFont = fontManager != nullptr ? fontManager->getFont("icons") : nullptr;

        // Push the icon font if available, otherwise use default
        if (iconFont != nullptr) {
            ImGui::PushFont(iconFont);
        }

        bool clicked = ImGui::Button(icon);

        if (iconFont != nullptr) {
            ImGui::PopFont();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip);
        }

        return clicked;
    }

    void ToolBarRenderer::renderFileButtons()
    {
        if (renderIconButton(ICON_FA_FILE_O, m_translationManager->_t("MENU.FILE_NEW").data())) {
            this->m_navigationService->fileNewHandler();
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_FOLDER_OPEN_O, m_translationManager->_t("MENU.FILE_OPEN").data())) {
            this->m_navigationService->fileOpenHandler();
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_FLOPPY_O, m_translationManager->_t("MENU.FILE_SAVE").data())) {
            // Handle save file
        }
    }

    void ToolBarRenderer::renderEditButtons()
    {
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        if (renderIconButton(ICON_FA_UNDO, m_translationManager->_t("MENU.EDIT_UNDO").data())) {
            // Handle undo
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_REPEAT, m_translationManager->_t("MENU.EDIT_REDO").data())) {
            // Handle redo
        }

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        if (renderIconButton(ICON_FA_SCISSORS, m_translationManager->_t("MENU.EDIT_CUT").data())) {
            // Handle cut
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_FILES_O, m_translationManager->_t("MENU.EDIT_COPY").data())) {
            // Handle copy
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_CLIPBOARD, m_translationManager->_t("MENU.EDIT_PASTE").data())) {
            // Handle paste
        }
    }

    void ToolBarRenderer::renderViewButtons()
    {
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        if (renderIconButton(ICON_FA_SEARCH_PLUS, m_translationManager->_t("MENU.VIEW_ZOOM_IN").data())) {
            // Handle zoom in
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_SEARCH_MINUS, m_translationManager->_t("MENU.VIEW_ZOOM_OUT").data())) {
            // Handle zoom out
        }

        ImGui::SameLine();
        if (renderIconButton(ICON_FA_REFRESH, m_translationManager->_t("MENU.VIEW_RESET_LAYOUT").data())) {
            m_layoutManager->resetLayout();
        }
    }

    void ToolBarRenderer::renderLanguageSelector()
    {
        // i18n::i18n* tm = this->getTranslationManager();
        //
        // ImGui::SameLine();
        // ImGui::Separator();
        // ImGui::SameLine();
        //
        // // Language selector with globe icon
        // if (ImGui::Button(ICON_FA_GLOBE)) {
        //     ImGui::OpenPopup(tm->_t("MENU.OPTIONS_LANGUAGE_SELECTOR").data());
        // }
        //
        // if (ImGui::IsItemHovered()) {
        //     ImGui::SetTooltip(static_cast<const char *>(tm->_t("MENU.OPTIONS_LANGUAGE_SELECTOR").data()));
        // }
        //
        // // Language selection popup
        // if (ImGui::BeginPopup("language_selector")) {
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::ENGLISH_UNITED_STATES).data())) {
        //         this->getTranslationManager()->setLocale(Constants::Languages::ENGLISH_UNITED_STATES.data());
        //     }
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::ENGLISH_UNITED_KINGDOM).data())) {
        //         this->getTranslationManager()->setLocale(Constants::Languages::ENGLISH_UNITED_KINGDOM.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::SPANISH_SPAIN).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::SPANISH_SPAIN.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::FRENCH_FRANCE).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::FRENCH_FRANCE.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::GERMAN_GERMANY).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::GERMAN_GERMANY.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::ITALIAN_ITALY).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::ITALIAN_ITALY.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::PORTUGUESE_PORTUGAL).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::PORTUGUESE_PORTUGAL.data());
        //     }
        //
        //     if (ImGui::MenuItem(Constants::Languages::getLanguageName(Constants::Languages::RUSSIAN_RUSSIA).data())) {
        //         this->getTranslationManager()->setLocale(ADS::Constants::Languages::RUSSIAN_RUSSIA.data());
        //     }
        //     ImGui::EndPopup();
        // }

        // Theme toggle
        // ImGui::SameLine();
        // if (renderIconButton(ICON_FA_MOON_O, tm->_t("MENU.VIEW_TOOLBAR_TOOGLE_THEME").data())) {
        //     // TODO: Implement proper theme toggle state tracking
        //     // For now, just toggle to dark theme
        //     handleThemeChange(true);
        // }
    }

    void ToolBarRenderer::handleThemeChange(bool darkTheme)
    {
        if (darkTheme) {
            DarkTheme theme;
            theme.apply();
        } else {
            LightTheme theme;
            theme.apply();
        }
    }

    void ToolBarRenderer::render()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Position toolbar right below the menu bar
        ImVec2 toolbarPos = ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight());
        ImVec2 toolbarSize = ImVec2(viewport->Size.x, getHeight());

        ImGui::SetNextWindowPos(toolbarPos);
        ImGui::SetNextWindowSize(toolbarSize);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoSavedSettings;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_buttonPadding, m_buttonPadding));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(m_buttonPadding, 0.0f));

        if (ImGui::Begin("##Toolbar", nullptr, windowFlags)) {
            renderFileButtons();
            renderEditButtons();
            renderViewButtons();
            // renderLanguageSelector();
        }

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    float ToolBarRenderer::getHeight() const
    {
        return m_buttonHeight + (m_buttonPadding * 2);
    }
}
