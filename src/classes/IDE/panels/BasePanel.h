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
         * @param name Window name for the panel
         */
        explicit BasePanel(const std::string &name);

        /**
         * @brief Destroy the BasePanel object
         */
        virtual ~BasePanel() = default;

        /**
         * @brief Render the panel (pure virtual method)
         *
         * Must be implemented by derived classes to handle panel rendering.
         */
        virtual void render() = 0;

        /**
         * @brief Set panel visibility
         *
         * @param visible True to show panel, false to hide
         */
        void setVisible(bool visible);

        /**
         * @brief Get panel visibility state
         *
         * @return true if panel is visible
         * @return false if panel is hidden
         */
        bool isVisible() const;

        /**
         * @brief Get the window name
         *
         * @return const std::string& Window name
         */
        const std::string &getWindowName() const;
    };
}

#endif //ADS_BASE_PANEL_H
