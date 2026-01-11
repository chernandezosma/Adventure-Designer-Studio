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

/**
 * @file BasePanel.cpp
 * @brief Implementation of the BasePanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "BasePanel.h"

#include "app.h"

namespace ADS::IDE::Panels {

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
    BasePanel::BasePanel(const std::string& name)
        : m_windowName(name), m_isVisible(true) {
        this->m_environment = Core::App::getEnv();
        this->m_translationsManager = Core::App::getTranslationsManager();
    }

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
    void BasePanel::setVisible(bool visible) {
        m_isVisible = visible;
    }

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
    bool BasePanel::isVisible() const {
        return m_isVisible;
    }

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
    const std::string& BasePanel::getWindowName() const {
        return m_windowName;
    }
}
