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
    /**
     * @brief Construct a new ToolBarRenderer object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the toolbar renderer with references to required services.
     * Sets up the layout manager reference, creates a new NavigationService
     * instance, obtains the translation manager from the IDEBase parent class,
     * and initializes button dimensions with default values (26.0f height,
     * 4.0f padding).
     *
     * @param layoutManager Pointer to the layout manager for layout operations
     *
     * @note Locale management is handled by the IDEBase parent class
     * @see IDEBase::IDEBase()
     */
    ToolBarRenderer::ToolBarRenderer(LayoutManager *layoutManager) :
        m_layoutManager(layoutManager),
        m_translationManager(this->getTranslationManager()),
        m_buttonHeight(26.0f),
        m_buttonPadding(4.0f)
    {
        // Locale is now managed in IDEBase
    }

    /**
     * @brief Render a toolbar button with an icon
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Creates and renders an icon button using FontAwesome icons. Retrieves the
     * icon font from the font manager (inherited from IDEBase) and temporarily
     * pushes it for button rendering. If the icon font is unavailable, uses the
     * default font. Displays a tooltip when the user hovers over the button.
     *
     * @param icon FontAwesome icon character constant (e.g., ICON_FA_FILE_O)
     * @param tooltip Tooltip text to display on hover
     *
     * @return true if the button was clicked, false otherwise
     *
     * @note The icon font must be loaded in the font manager for proper rendering
     * @see UI::Fonts::getFont()
     * @see IDEBase::getFontManager()
     */
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

    /**
     * @brief Render file operation buttons
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders toolbar buttons for file operations arranged horizontally:
     * - New (ICON_FA_FILE_O): Creates a new file via NavigationService
     * - Open (ICON_FA_FOLDER_OPEN_O): Opens an existing file via NavigationService
     * - Save (ICON_FA_FLOPPY_O): Saves the current file (placeholder implementation)
     *
     * All button tooltips are retrieved from the translation manager for i18n support.
     *
     * @see NavigationService::fileNewHandler()
     * @see NavigationService::fileOpenHandler()
     * @see renderIconButton()
     */
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

    /**
     * @brief Render edit operation buttons
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders toolbar buttons for editing operations arranged horizontally with separators:
     * - Undo (ICON_FA_UNDO): Undo last action (placeholder implementation)
     * - Redo (ICON_FA_REPEAT): Redo previously undone action (placeholder implementation)
     * - Cut (ICON_FA_SCISSORS): Cut selection to clipboard (placeholder implementation)
     * - Copy (ICON_FA_FILES_O): Copy selection to clipboard (placeholder implementation)
     * - Paste (ICON_FA_CLIPBOARD): Paste from clipboard (placeholder implementation)
     *
     * Buttons are grouped with visual separators between different operation types.
     * All button tooltips are retrieved from the translation manager for i18n support.
     *
     * @note All edit operations currently contain placeholder implementations
     * @see renderIconButton()
     */
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

    /**
     * @brief Render view control buttons
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders toolbar buttons for view controls arranged horizontally with a separator:
     * - Zoom In (ICON_FA_SEARCH_PLUS): Increase view zoom level (placeholder implementation)
     * - Zoom Out (ICON_FA_SEARCH_MINUS): Decrease view zoom level (placeholder implementation)
     * - Reset Layout (ICON_FA_REFRESH): Restores default IDE layout via LayoutManager
     *
     * All button tooltips are retrieved from the translation manager for i18n support.
     *
     * @see LayoutManager::resetLayout()
     * @see renderIconButton()
     */
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

    /**
     * @brief Render language selector
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders the language selector button and popup menu. The entire implementation
     * is currently commented out. When enabled, it would provide:
     * - A globe icon button (ICON_FA_GLOBE) that opens a popup menu
     * - A popup menu with language options for multiple locales
     * - Theme toggle button (ICON_FA_MOON_O) for switching between themes
     *
     * @note Currently not implemented - all code is commented out
     * @see renderIconButton()
     * @see handleThemeChange()
     */
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

    /**
     * @brief Handle theme change
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Applies the specified theme to the ImGui interface. Creates an instance of
     * either DarkTheme or LightTheme based on the parameter and calls its apply()
     * method to update the ImGui style colors and visual appearance.
     *
     * @param darkTheme True to apply dark theme, false to apply light theme
     *
     * @see DarkTheme::apply()
     * @see LightTheme::apply()
     */
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

    /**
     * @brief Render the complete toolbar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders the complete toolbar as a fixed-position ImGui window. The toolbar
     * is positioned immediately below the menu bar, spanning the full width of the
     * viewport. Creates a borderless, immovable window with custom styling and
     * renders all toolbar button groups.
     *
     * The window uses the following flags:
     * - NoTitleBar: Removes the title bar
     * - NoResize: Prevents manual resizing
     * - NoMove: Prevents manual repositioning
     * - NoScrollbar: Disables scrollbars
     * - NoSavedSettings: Prevents saving window state
     *
     * Custom styling is applied via PushStyleVar for window padding and item spacing.
     *
     * @see renderFileButtons()
     * @see renderEditButtons()
     * @see renderViewButtons()
     * @see getHeight()
     */
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

    /**
     * @brief Get the height of the toolbar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Calculates and returns the total height of the toolbar window including
     * padding. The calculation adds the button height to twice the padding value
     * (once for top padding and once for bottom padding).
     *
     * @return float Total toolbar height in pixels (button height + 2 * padding)
     *
     * @note This value is used for positioning elements below the toolbar and
     *       for setting the toolbar window size
     */
    float ToolBarRenderer::getHeight() const
    {
        return m_buttonHeight + (m_buttonPadding * 2);
    }
}
