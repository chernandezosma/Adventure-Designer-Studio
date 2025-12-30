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
         */
        void renderTabBar();

        /**
         * @brief Render the script editor
         */
        void renderScriptEditor();

        /**
         * @brief Handle new tab creation
         */
        void handleNewTab();

    public:
        /**
         * @brief Construct a new WorkingAreaPanel object
         */
        WorkingAreaPanel();

        /**
         * @brief Destroy the WorkingAreaPanel object
         */
        ~WorkingAreaPanel() override = default;

        /**
         * @brief Render the working area panel
         *
         * Displays a tabbed interface for editing scripts with
         * multi-line text editors.
         */
        void render() override;
    };
}

#endif //ADS_WORKING_AREA_PANEL_H
