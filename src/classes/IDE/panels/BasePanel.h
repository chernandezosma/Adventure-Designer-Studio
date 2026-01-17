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


#ifndef ADS_BASE_PANEL_H
#define ADS_BASE_PANEL_H

#include <string>

#include "env/env.h"
#include "i18n/i18n.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Abstract base class for all IDE panels
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides common functionality for all IDE UI panels including
     * visibility management and window naming. All concrete panel classes
     * must implement the render() method.
     */
    class BasePanel
    {
    protected:
        /**
         * Translations manager
         */
        i18n::i18n *m_translationsManager;

        /**
         * Environment class to manage the .env content.
         */
        Environment *m_environment;

        /**
         * Window name displayed in ImGui
         */
        std::string m_windowName;

        /**
         * Panel visibility state
         */
        bool m_isVisible;

    public:
        /**
         * @brief Construct a new BasePanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the base panel with the specified window name and sets
         * default visibility to true. Also initializes environment and translations
         * manager references from the application instance.
         *
         * @param name Window name for the panel
         */
        explicit BasePanel(const std::string &name);

        /**
         * @brief Destroy the BasePanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Virtual destructor to ensure proper cleanup of derived classes.
         * Default implementation is sufficient as this class uses only
         * non-owning pointers.
         */
        virtual ~BasePanel() = default;

        /**
         * @brief Render the panel (pure virtual method)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Must be implemented by derived classes to handle panel rendering.
         * This method is called every frame when the panel should be drawn.
         * Implementations should check visibility state and handle ImGui
         * rendering calls.
         */
        virtual void render() = 0;

        /**
         * @brief Set panel visibility
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Controls whether the panel should be rendered. When set to false,
         * the panel's render() method should return early without drawing.
         *
         * @param visible True to show panel, false to hide
         */
        void setVisible(bool visible);

        /**
         * @brief Get panel visibility state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the current visibility state of the panel. This does not
         * necessarily mean the panel is currently being displayed, only that
         * it is marked as visible.
         *
         * @return true if panel is visible
         * @return false if panel is hidden
         */
        bool isVisible() const;

        /**
         * @brief Get the window name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the name used to identify this panel's window in ImGui.
         * This name is displayed in the window title bar and used internally
         * by ImGui for window management.
         *
         * @return const std::string& Window name reference
         */
        const std::string &getWindowName() const;
    };
}

#endif //ADS_BASE_PANEL_H
