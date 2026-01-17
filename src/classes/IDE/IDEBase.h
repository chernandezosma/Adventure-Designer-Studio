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


#ifndef ADS_IDE_BASE_H
#define ADS_IDE_BASE_H
#include "env/env.h"
#include "i18n/i18n.h"
#include "UI/fonts.h"

namespace ADS::IDE {
    /**
     * @brief Base class for IDE components with shared infrastructure
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides foundational functionality for IDE-related components by managing
     * access to core application services including internationalization and
     * environment configuration. Serves as a base class for IDE subsystems that
     * require translation and configuration capabilities.
     *
     * This class maintains references to the application's translation manager
     * and environment configuration, making them easily accessible to derived
     * IDE components without tight coupling to the main App class.
     */
    class IDEBase
    {
    private:
        /**
         * @brief Translations manager for internationalization support
         *
         * Provides access to localized strings and language management for
         * IDE components. Enables multi-language support throughout the
         * user interface.
         */
        i18n::i18n *m_translationsManager;

        /**
         * @brief Environment configuration manager
         *
         * Manages application configuration loaded from .env files.
         * Provides access to environment variables and settings needed
         * by IDE components.
         */
        Environment *m_environment;

        /**
         * @brief Font manager for loading and accessing fonts
         *
         * Provides access to the application's font manager for
         * retrieving fonts by name, including icon fonts.
         */
        UI::Fonts *m_fontManager;

        /**
         * @brief Current locale information
         *
         * Stores the current locale information including locale code
         * and language name. Cached to avoid repeated calls to the
         * translation manager.
         */
        i18n::LocaleInfo m_locale;

    public:
        /**
         * @brief Construct IDE base component
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Initializes the IDE base component by obtaining references to the
         * application's translation manager and environment configuration through
         * the App class static methods. These references are cached for efficient
         * access by derived classes.
         *
         * @see App::getTranslationsManager()
         * @see App::getEnv()
         */
        IDEBase();

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
        i18n::i18n *getTranslationManager();

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
        Environment *getEnvironment();

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
        UI::Fonts *getFontManager();

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
        const i18n::LocaleInfo& getLocale() const;

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
        void updateLocale();
    };
} // ADS

#endif // ADS_IDE_BASE_H
