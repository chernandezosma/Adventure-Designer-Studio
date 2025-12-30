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
         */
        void renderSelectionInfo();

        /**
         * @brief Render metadata information
         */
        void renderMetadata();

    public:
        /**
         * @brief Construct a new InspectorPanel object
         */
        InspectorPanel();

        /**
         * @brief Destroy the InspectorPanel object
         */
        ~InspectorPanel() override = default;

        /**
         * @brief Render the inspector panel
         *
         * Displays detailed information about the selected entity.
         */
        void render() override;
    };
}

#endif //ADS_INSPECTOR_PANEL_H
