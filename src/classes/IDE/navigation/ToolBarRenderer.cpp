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
     * @brief Render toolbar content inline (without creating a window)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders the toolbar content directly into the current ImGui context.
     * This method should be called inside an existing ImGui window (such as
     * the main dockspace window) after the menu bar but before the DockSpace
     * call. This allows ImGui to automatically account for the toolbar height
     * when calculating available space for docked panels.
     *
     * The toolbar is rendered as a child region with fixed height, spanning
     * the full available width. Custom styling is applied for item spacing.
     *
     * @see renderFileButtons()
     * @see renderEditButtons()
     * @see renderViewButtons()
     * @see getHeight()
     */
    void ToolBarRenderer::renderContent()
    {
        // Push styling for the toolbar content
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(m_buttonPadding, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_buttonPadding, m_buttonPadding));

        // Create a child region for the toolbar with fixed height
        ImGui::BeginChild("##ToolbarContent", ImVec2(0, getHeight()), false, ImGuiWindowFlags_NoScrollbar);

        renderFileButtons();
        renderEditButtons();
        renderViewButtons();
        // renderLanguageSelector();

        ImGui::EndChild();
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
     *
     * @todo Jan 22, 2026 <chernandez@elestadoweb.com>
     *       Probably we will need to detect the screen DPI and based the space
     *       on that value.
     */
    float ToolBarRenderer::getHeight() const
    {
        float padding = m_buttonPadding * 2.0f;

        // 22 Jan 2026 <chernandez@elestadoweb.com>
        // This patch is applied because the space between toolbar and docking windows
        // are treated in different way if you are in Windows (add extra space) or Linux,
        // remove the space.
#ifdef _WIN32
        padding = 0.0f;
#endif

        return m_buttonHeight + padding;
    }
}
