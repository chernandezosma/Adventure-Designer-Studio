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

    /**
     * @brief Get the translation manager instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides access to the i18n translation manager for retrieving
     * localized strings and managing language settings within IDE components.
     *
     * @return Pointer to the i18n instance for translation operations
     *
     * @see i18n::i18n
     */
    i18n::i18n * IDEBase::getTranslationManager()
    {
        return this->m_translationsManager;
    }

    /**
     * @brief Get the environment configuration instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides access to the Environment configuration manager for reading
     * application settings and environment variables within IDE components.
     *
     * @return Pointer to the Environment instance for configuration access
     *
     * @see Environment
     */
    Environment * IDEBase::getEnvironment()
    {
        return this->m_environment;
    }

    /**
     * @brief Get the font manager instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides access to the font manager for retrieving fonts by name,
     * including icon fonts, within IDE components.
     *
     * @return Pointer to the Fonts manager instance
     *
     * @see UI::Fonts
     */
    UI::Fonts * IDEBase::getFontManager()
    {
        // Lazy-load font manager on first access
        if (this->m_fontManager == nullptr) {
            this->m_fontManager = Core::App::getFontManager();
        }
        return this->m_fontManager;
    }

    /**
     * @brief Get the current locale information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Returns the cached locale information including locale code and
     * language name. This avoids repeated calls to the translation manager.
     *
     * @return Reference to the current LocaleInfo
     *
     * @see i18n::LocaleInfo
     */
    const i18n::LocaleInfo& IDEBase::getLocale() const
    {
        return this->m_locale;
    }

    /**
     * @brief Update the cached locale information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Refreshes the cached locale information from the translation manager.
     * Should be called when the language is changed to keep the cache
     * synchronized.
     */
    void IDEBase::updateLocale()
    {
        this->m_locale = this->m_translationsManager->getCurrentLocale();
    }


} // ADS
