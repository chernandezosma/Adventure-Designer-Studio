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


#ifndef ADS_WORKING_AREA_PANEL_H
#define ADS_WORKING_AREA_PANEL_H

#include "BasePanel.h"
#include <string>

namespace ADS::IDE::Panels {
    /**
     * @brief Working area panel for editing scripts and documents
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides a tabbed interface for editing multiple scripts and documents
     * with multi-line text editors and tab management.
     */
    class WorkingAreaPanel : public BasePanel {
    private:
        /**
         * Script text buffer for Script 1
         */
        char m_scriptText[4096];

        /**
         * @brief Render the tab bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays a tabbed interface for managing multiple open documents.
         * Shows tabs for each open script and provides a "+" tab for creating
         * new scripts. Each tab can contain different content.
         *
         * @note Currently displays placeholder tabs: Script 1, Script 2, and +
         * @see renderScriptEditor()
         */
        void renderTabBar();

        /**
         * @brief Render the script editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays a multi-line text editor for editing script content.
         * The editor fills the available space in the tab and allows
         * free-form text input with FontAwesome icons support.
         *
         * @note Currently edits the m_scriptText buffer
         */
        void renderScriptEditor();

        /**
         * @brief Handle new tab creation
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Handles the user action when the "+" tab is clicked.
         * This method will be responsible for creating new script tabs
         * in the working area.
         *
         * @note Currently a placeholder implementation
         */
        void handleNewTab();

    public:
        /**
         * @brief Construct a new WorkingAreaPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the working area panel with name "Working Area" and
         * sets up default script text with sample content including scene
         * description, dialog, and FontAwesome icons.
         */
        WorkingAreaPanel();

        /**
         * @brief Destroy the WorkingAreaPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~WorkingAreaPanel() override = default;

        /**
         * @brief Render the working area panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays a tabbed interface for editing scripts with
         * multi-line text editors. The panel provides the main content
         * editing area with support for multiple open documents.
         *
         * @note Returns early if panel is not visible
         * @see renderTabBar(), renderScriptEditor()
         */
        void render() override;
    };
}

#endif //ADS_WORKING_AREA_PANEL_H
