/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
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
