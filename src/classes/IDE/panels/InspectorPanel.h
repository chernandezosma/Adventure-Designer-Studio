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


#ifndef ADS_INSPECTOR_PANEL_H
#define ADS_INSPECTOR_PANEL_H

#include "BasePanel.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Inspector panel for displaying detailed entity information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Shows detailed information about the currently selected entity
     * including its type, location, and other metadata.
     */
    class InspectorPanel : public BasePanel {
    private:
        /**
         * @brief Render selection information
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays basic information about the currently selected entity
         * including its name, type, and location. Shows "None" if no
         * entity is currently selected.
         *
         * @note Currently displays placeholder data
         */
        void renderSelectionInfo();

        /**
         * @brief Render metadata information
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays additional metadata and detailed information about
         * the selected entity. Shows a message prompting the user to
         * select an entity if none is selected.
         *
         * @note Currently displays placeholder data
         */
        void renderMetadata();

    public:
        /**
         * @brief Construct a new InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the inspector panel with name "Inspector".
         */
        InspectorPanel();

        /**
         * @brief Destroy the InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~InspectorPanel() override = default;

        /**
         * @brief Render the inspector panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays detailed information about the selected entity,
         * including selection details and metadata. The panel provides
         * an overview of the currently focused entity in the IDE.
         *
         * @note Returns early if panel is not visible
         * @see renderSelectionInfo(), renderMetadata()
         */
        void render() override;
    };
}

#endif //ADS_INSPECTOR_PANEL_H
