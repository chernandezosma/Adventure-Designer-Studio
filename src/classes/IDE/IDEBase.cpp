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


#include "IDEBase.h"

#include "app.h"

namespace ADS::IDE {

    IDEBase::IDEBase()
    {
        this->m_environment = Core::App::getEnv();
        this->m_translationsManager = Core::App::getTranslationsManager();
        // Font manager is lazy-loaded in getFontManager() because it's not available during construction
        this->m_fontManager = nullptr;
        // Initialize locale cache
        this->m_locale = this->m_translationsManager->getCurrentLocale();
    }

    i18n::i18n * IDEBase::getTranslationManager()
    {
        return this->m_translationsManager;
    }

    Environment * IDEBase::getEnvironment()
    {
        return this->m_environment;
    }

    UI::Fonts * IDEBase::getFontManager()
    {
        // Lazy-load font manager on first access
        if (this->m_fontManager == nullptr) {
            this->m_fontManager = Core::App::getFontManager();
        }
        return this->m_fontManager;
    }

    const i18n::LocaleInfo& IDEBase::getLocale() const
    {
        return this->m_locale;
    }

    void IDEBase::updateLocale()
    {
        this->m_locale = this->m_translationsManager->getCurrentLocale();
    }


} // ADS
