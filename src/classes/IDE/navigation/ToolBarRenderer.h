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
         * @brief Layout manager reference for actions like reset layout
         *
         * Provides access to layout management functionality for operations
         * such as resetting the IDE layout to default settings.
         */
        LayoutManager *m_layoutManager;

        /**
         * @brief Service for handling navigation actions
         *
         * Unique pointer to NavigationService that handles file operations
         * and other navigation-related actions triggered from toolbar buttons.
         */
        std::unique_ptr<NavigationService> m_navigationService;

        /**
         * @brief Translation manager for internationalization
         *
         * Provides access to localized strings for button tooltips and labels,
         * enabling multi-language support in the toolbar.
         */
        i18n::i18n* m_translationManager;

        /**
         * @brief Toolbar button height in pixels
         *
         * Defines the vertical size of toolbar buttons. Used to calculate
         * the overall toolbar height.
         */
        float m_buttonHeight;

        /**
         * @brief Toolbar button padding in pixels
         *
         * Defines the spacing around toolbar buttons. Used for both
         * window padding and item spacing.
         */
        float m_buttonPadding;

        /**
         * @brief Render a toolbar button with an icon
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Creates and renders an icon button using FontAwesome icons. The button
         * uses the icon font from the font manager if available, otherwise falls
         * back to the default font. Displays a tooltip when the user hovers over
         * the button.
         *
         * @param icon FontAwesome icon character constant (e.g., ICON_FA_FILE_O)
         * @param tooltip Tooltip text to display on hover
         *
         * @return true if the button was clicked, false otherwise
         *
         * @note The icon font must be loaded in the font manager
         * @see UI::Fonts::getFont()
         */
        bool renderIconButton(const char *icon, const char *tooltip);

        /**
         * @brief Render file operation buttons
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders toolbar buttons for file operations including New, Open, and Save.
         * Delegates file operations to the NavigationService. Buttons are rendered
         * horizontally with SameLine() between them.
         *
         * @see NavigationService::fileNewHandler()
         * @see NavigationService::fileOpenHandler()
         * @see renderIconButton()
         */
        void renderFileButtons();

        /**
         * @brief Render edit operation buttons
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders toolbar buttons for editing operations including Undo, Redo,
         * Cut, Copy, and Paste. Currently contains placeholder implementations.
         * Buttons are grouped with separators and rendered horizontally.
         *
         * @note All edit operations currently contain placeholder implementations
         * @see renderIconButton()
         */
        void renderEditButtons();

        /**
         * @brief Render view control buttons
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders toolbar buttons for view controls including Zoom In, Zoom Out,
         * and Reset Layout. The Reset Layout button delegates to the LayoutManager.
         * Buttons are grouped with a separator and rendered horizontally.
         *
         * @see LayoutManager::resetLayout()
         * @see renderIconButton()
         */
        void renderViewButtons();

        /**
         * @brief Render language selector
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders the language selector button and popup menu. Currently commented
         * out in the implementation. Would provide a globe icon button that opens
         * a popup menu for selecting the application language.
         *
         * @note Currently not implemented (code is commented out)
         * @see renderIconButton()
         */
        void renderLanguageSelector();

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
        void handleThemeChange(bool darkTheme);

    public:
        /**
         * @brief Construct a new ToolBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the toolbar renderer with references to required services.
         * Sets up the layout manager, creates the navigation service, obtains
         * the translation manager from IDEBase, and initializes button dimensions
         * (26.0f height, 4.0f padding).
         *
         * @param layoutManager Pointer to the layout manager for layout operations
         *
         * @note The translation manager is obtained through IDEBase inheritance
         * @see IDEBase::getTranslationManager()
         */
        explicit ToolBarRenderer(LayoutManager *layoutManager);

        /**
         * @brief Destroy the ToolBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. The NavigationService is automatically cleaned up
         * through the unique_ptr.
         */
        ~ToolBarRenderer() = default;

        /**
         * @brief Render the complete toolbar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders the complete toolbar as a fixed-position window below the menu bar.
         * Creates a borderless, immovable window with custom padding and spacing,
         * then renders all toolbar button groups (file, edit, view).
         *
         * The toolbar is positioned at the top of the viewport, immediately below
         * the menu bar, and spans the full width of the window.
         *
         * @note Creates its own ImGui window with specific flags and styling
         * @see renderFileButtons()
         * @see renderEditButtons()
         * @see renderViewButtons()
         */
        void render();

        /**
         * @brief Get the height of the toolbar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Calculates and returns the total height of the toolbar including padding.
         * The height is computed as button height plus twice the padding (top and bottom).
         *
         * @return float Toolbar height in pixels (button height + 2 * padding)
         *
         * @note Used for layout calculations to position elements below the toolbar
         */
        float getHeight() const;
    };
}

#endif //ADS_TOOLBAR_RENDERER_H
