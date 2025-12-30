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
    private:
        /**
         * Height of the status bar in pixels
         */
        float m_height;

        /**
         * @brief Calculate the height of the status bar
         *
         * Calculates the height based on ImGui frame height and spacing.
         */
        void calculateHeight();

    public:
        /**
         * @brief Construct a new StatusBarPanel object
         */
        StatusBarPanel();

        /**
         * @brief Destroy the StatusBarPanel object
         */
        ~StatusBarPanel() override = default;

        /**
         * @brief Render the status bar
         *
         * Displays the status bar at the bottom of the viewport with
         * status information, cursor position, and FPS counter.
         */
        void render() override;

        /**
         * @brief Get the height of the status bar
         *
         * @return float Height in pixels
         */
        float getHeight() const;
    };
}

#endif //ADS_STATUS_BAR_PANEL_H
