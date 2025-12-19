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

#include "LayoutManager.h"

namespace ADS::IDE {
    /**
     * @brief Renders the main menu bar for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides menu functionality for File, View, and Help menus
     * including theme switching and layout management.
     */
    class MenuBarRenderer {
    private:
        /**
         * Reference to the layout manager
         */
        LayoutManager* m_layoutManager;

        /**
         * @brief Render the File menu
         *
         * Includes New, Open, Save, and Exit menu items.
         */
        void renderFileMenu();

        /**
         * @brief Render the View menu
         *
         * Includes Reset Layout and Theme submenu.
         */
        void renderViewMenu();

        /**
         * @brief Render the Help menu
         *
         * Includes About menu item.
         */
        void renderHelpMenu();

        /**
         * @brief Handle exit action
         *
         * Sends SDL quit event to close the application.
         */
        void handleExit();

        /**
         * @brief Handle theme change
         *
         * @param darkTheme True for dark theme, false for light theme
         */
        void handleThemeChange(bool darkTheme);

    public:
        /**
         * @brief Construct a new MenuBarRenderer object
         *
         * @param layoutManager Pointer to the layout manager
         */
        explicit MenuBarRenderer(LayoutManager* layoutManager);

        /**
         * @brief Destroy the MenuBarRenderer object
         */
        ~MenuBarRenderer() = default;

        /**
         * @brief Render the menu bar
         *
         * Renders the complete menu bar with File, View, and Help menus.
         * Should be called within an ImGui::BeginMenuBar() context.
         */
        void render();
    };
}

#endif //ADS_MENU_BAR_RENDERER_H
