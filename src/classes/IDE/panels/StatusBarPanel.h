/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
 */


#ifndef ADS_STATUS_BAR_PANEL_H
#define ADS_STATUS_BAR_PANEL_H

#include "BasePanel.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Status bar panel displayed at the bottom of the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Renders a fixed status bar at the bottom of the viewport displaying
     * information such as ready state, cursor position, FPS counter, and
     * application name.
     */
    class StatusBarPanel : public BasePanel {
    public:
        /**
         * @brief Construct a new StatusBarPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the status bar panel with name "Status Bar" and
         * sets initial height to 0.0f. The actual height is calculated
         * dynamically during rendering.
         */
        /**
         * @brief Construct a new StatusBarPanel.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         */
        StatusBarPanel();

        /**
         * @brief Default destructor.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         */
        ~StatusBarPanel() override = default;

        /**
         * @brief Render the status bar for the current frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Positions a fixed 22-px window at the bottom of the main viewport.
         * Left side: version · project name · active file · cursor position.
         * Right side: entity counts and error / warning counts.
         */
        void render() override;

        /**
         * @brief Return the fixed height of the status bar in pixels.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return float Always 22.0f as per the design specification.
         */
        float getHeight() const;

        /**
         * @brief Set the display name of the active project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param name Project name shown in the status bar.
         */
        void setProjectName(const std::string& name);

        /**
         * @brief Set the name of the currently open file.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param filename File name (not full path) shown in the status bar.
         */
        void setActiveFile(const std::string& filename);

        /**
         * @brief Set the editor cursor position displayed in the status bar.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param line 1-based line number.
         * @param col  1-based column number.
         */
        void setCursorPosition(int line, int col);

        /**
         * @brief Set the entity and diagnostic counts shown on the right side.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param scenes   Number of scenes in the project.
         * @param npcs     Number of NPCs in the project.
         * @param warnings Number of compiler / lint warnings.
         * @param errors   Number of compiler / lint errors.
         */
        void setCounts(int scenes, int npcs, int warnings, int errors);

    private:
        std::string m_projectName  = "Sin proyecto";
        std::string m_activeFile;
        int         m_line         = 1;
        int         m_col          = 1;
        int         m_scenes       = 0;
        int         m_npcs         = 0;
        int         m_warnings     = 0;
        int         m_errors       = 0;
    };
}

#endif //ADS_STATUS_BAR_PANEL_H
