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
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Calculates the height based on ImGui frame height and spacing.
         * This method should be called during rendering to update the
         * height based on current ImGui style settings.
         *
         * @note This method updates the m_height member variable
         */
        void calculateHeight();

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
        StatusBarPanel();

        /**
         * @brief Destroy the StatusBarPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~StatusBarPanel() override = default;

        /**
         * @brief Render the status bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays the status bar at the bottom of the viewport with
         * status information, cursor position, and FPS counter. The bar
         * is positioned as a fixed window at the bottom with no title bar,
         * borders, or docking capabilities.
         *
         * @note Returns early if panel is not visible
         * @see calculateHeight()
         */
        void render() override;

        /**
         * @brief Get the height of the status bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the current calculated height of the status bar. This
         * value is used by other components to properly size and position
         * their content relative to the status bar.
         *
         * @return float Height in pixels
         * @note The height is calculated during each render() call
         */
        float getHeight() const;
    };
}

#endif //ADS_STATUS_BAR_PANEL_H
