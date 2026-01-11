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


#ifndef ADS_TOOLBAR_RENDERER_H
#define ADS_TOOLBAR_RENDERER_H

#include "NavigationConstants.h"
#include "NavigationService.h"
#include "../IDEBase.h"
#include "../LayoutManager.h"

namespace ADS::IDE {
    /**
     * @brief Renders the toolbar with icon buttons
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides a horizontal toolbar with icon buttons for common actions:
     * - File operations (New, Open, Save)
     * - Edit operations (Undo, Redo, Cut, Copy, Paste)
     * - View controls (Zoom In, Zoom Out, Reset Layout)
     * - Language selector
     */
    class ToolBarRenderer : public IDEBase {
    private:

        /**
         * Layout manager reference for actions like reset layout
         */
        LayoutManager *m_layoutManager;

        /**
         * Service for the handler functions
         */
        std::unique_ptr<NavigationService> m_navigationService;

        /**
         * Translation manager for internationalization
         */
        i18n::i18n* m_translationManager;

        /**
         * Toolbar button height
         */
        float m_buttonHeight;

        /**
         * Toolbar button padding
         */
        float m_buttonPadding;

        /**
         * @brief Render a toolbar button with an icon
         *
         * @param icon Icon character (FontAwesome)
         * @param tooltip Tooltip text to display on hover
         * @return true if the button was clicked
         */
        bool renderIconButton(const char *icon, const char *tooltip);

        /**
         * @brief Render file operation buttons
         */
        void renderFileButtons();

        /**
         * @brief Render edit operation buttons
         */
        void renderEditButtons();

        /**
         * @brief Render view control buttons
         */
        void renderViewButtons();

        /**
         * @brief Render language selector
         */
        void renderLanguageSelector();

        /**
         * @brief Handle theme change
         *
         * @param darkTheme true for dark theme, false for light theme
         */
        void handleThemeChange(bool darkTheme);

    public:
        /**
         * @brief Construct a new ToolBarRenderer object
         *
         * @param layoutManager Pointer to layout manager
         */
        explicit ToolBarRenderer(LayoutManager *layoutManager);

        /**
         * @brief Destroy the ToolBarRenderer object
         */
        ~ToolBarRenderer() = default;

        /**
         * @brief Render the complete toolbar
         */
        void render();

        /**
         * @brief Get the height of the toolbar
         *
         * @return float Toolbar height in pixels
         */
        float getHeight() const;
    };
}

#endif //ADS_TOOLBAR_RENDERER_H
