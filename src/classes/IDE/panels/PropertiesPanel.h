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
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Handles the user action when the "Apply" button is clicked.
         * This method will be responsible for applying the current property
         * changes to the selected object.
         *
         * @note Currently a placeholder implementation
         */
        void handleApply();

        /**
         * @brief Handle reset button click
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Handles the user action when the "Reset" button is clicked.
         * This method will be responsible for resetting properties to their
         * default or previous values.
         *
         * @note Currently a placeholder implementation
         */
        void handleReset();

    public:
        /**
         * @brief Construct a new PropertiesPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the properties panel with name "Properties" and
         * sets default values for all property fields. The name is set to
         * "Object Name", description to "Object description...", property
         * value to 50, and both visibility and interactive flags to true.
         */
        PropertiesPanel();

        /**
         * @brief Destroy the PropertiesPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~PropertiesPanel() override = default;

        /**
         * @brief Render the properties panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays property editors for name, description, value,
         * and various flags with apply and reset buttons. The panel
         * provides input controls for modifying object properties.
         *
         * @note Returns early if panel is not visible
         * @see handleApply(), handleReset()
         */
        void render() override;
    };
}

#endif //ADS_PROPERTIES_PANEL_H
