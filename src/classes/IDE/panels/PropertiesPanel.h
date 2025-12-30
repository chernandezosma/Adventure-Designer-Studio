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


#ifndef ADS_PROPERTIES_PANEL_H
#define ADS_PROPERTIES_PANEL_H

#include "BasePanel.h"
#include <cstring>

namespace ADS::IDE::Panels {
    /**
     * @brief Properties panel for editing object properties
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides an interface for editing properties of selected objects
     * including name, description, values, and various flags.
     */
    class PropertiesPanel : public BasePanel {
    private:
        /**
         * Object name buffer
         */
        char m_name[128];

        /**
         * Object description buffer
         */
        char m_description[256];

        /**
         * Property value slider
         */
        int m_propertyValue;

        /**
         * Visibility flag
         */
        bool m_isVisibleFlag;

        /**
         * Interactive flag
         */
        bool m_isInteractiveFlag;

        /**
         * @brief Handle apply button click
         */
        void handleApply();

        /**
         * @brief Handle reset button click
         */
        void handleReset();

    public:
        /**
         * @brief Construct a new PropertiesPanel object
         */
        PropertiesPanel();

        /**
         * @brief Destroy the PropertiesPanel object
         */
        ~PropertiesPanel() override = default;

        /**
         * @brief Render the properties panel
         *
         * Displays property editors for name, description, value,
         * and various flags with apply and reset buttons.
         */
        void render() override;
    };
}

#endif //ADS_PROPERTIES_PANEL_H
