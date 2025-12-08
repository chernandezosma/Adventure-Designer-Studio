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


#include "app.h"

#include "adsString.h"
#include "i18nUtils.h"
#include "Logger/logger.h"

namespace ADS::Core {
    void App::init()
    {
        Logger::init();
        spdlog::info(format("{}:{} - Retrieve the languages from .env file", __FILE__, __LINE__));
        const std::string* languagesAllowedFromEnv = this->environment->get("LANGUAGES");
        spdlog::info("Load the languages");

        for (const std::vector<std::string> languages = explode(*languagesAllowedFromEnv, ',');
             const string& language: languages) {
            this->translations.addLanguage(std::string(language));
        }
        spdlog::info(format("{}:{} - Using im GUI Library", __FILE__, __LINE__));
        this->imguiObject = UI::ImGuiManager();
    }

    /**
     * @brief Construct App instance and initialize environment
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Creates a new App instance and allocates an Environment object
     * to manage application configuration from .env files and initializes
     * the translation system.
     */
    App::App()
        : translations(
            "public/translations/core",
            std::string(ADS::Constants::Languages::ENGLISH_UNITED_STATES)
        )
    {
        this->environment = new ADS::Environment();
        this->init();
    }

    /**
     * @brief Destroy App instance and release resources
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Deallocates the Environment object to prevent memory leaks.
     */
    App::~App()
    {
        delete this->environment;
    }

    /**
     * @brief Retrieve the application environment manager
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the internal Environment object for reading
     * configuration values from .env files.
     *
     * @return Pointer to the ADS::Environment instance
     *
     * @warning The returned pointer is only valid while the App
     *          object exists
     */
    ADS::Environment* App::getEnv() const
    {
        return this->environment;
    }

    /**
     * @brief Get the logger instance for app-wide usage
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the initialized logger that can be used
     * throughout the application. Allows other components to use
     * the logger without direct dependency on the Logger class.
     *
     * @return Shared pointer to the spdlog logger instance
     *
     * @see ADS::Core::Logger::getInstance()
     */
    std::shared_ptr<spdlog::logger> App::getLogger()
    {
        return ADS::Core::Logger::getInstance();
    }

    /**
     * @brief Get the translations instance for app-wide usage
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the initialized translation system that can be
     * used throughout the application for localized text.
     *
     * @return Reference to the i18n translations instance
     *
     * @see i18n::i18n
     */
    i18n::i18n& App::getTranslations()
    {
        return this->translations;
    }

    /**
     * @brief Return the translations for the text on selected language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * @param text          Key to translate. It must exist in translations
     * @param language      Language used to get the translation
     *
     * @return Reference to the i18n translations instance
     *
     * @see i18n::i18n
     */
    string App::_t(const string& text, const string& language) const
    {
        return this->translations.translate(text, language);
    }

    /**
     * @brief Check if application is running in debug mode
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Delegates to the Environment object to determine if the DEBUG
     * variable is enabled in the configuration.
     *
     * @return true if DEBUG is enabled in the environment
     * @return false if DEBUG is disabled or not set
     *
     * @see ADS::Environment::isDebug()
     */
    bool App::isDebug()
    {
        return this->environment->isDebug();
    }

    [[nodiscard]] Environment* App::getEnvironment()
    {
        return environment;
    }

    [[nodiscard]] i18n::i18n& App::getTranslatons()
    {
        return this->translations;
    }

    [[nodiscard]] UI::ImGuiManager& App::getImGuiObject()
    {
        return this->imguiObject;
    }
} // ADS