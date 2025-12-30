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


#include "BasePanel.h"

#include "app.h"

namespace ADS::IDE::Panels {

    BasePanel::BasePanel(const std::string& name)
        : m_windowName(name), m_isVisible(true) {
        this->m_environment = Core::App::getEnv();
        this->m_translationsManager = Core::App::getTranslationsManager();
    }

    void BasePanel::setVisible(bool visible) {
        m_isVisible = visible;
    }

    bool BasePanel::isVisible() const {
        return m_isVisible;
    }

    const std::string& BasePanel::getWindowName() const {
        return m_windowName;
    }
}
