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


#ifndef ADS_APP_H
#define ADS_APP_H

#include "env/env.h"
#include <spdlog/spdlog.h>

#include "UI/UI.h"
#include "i18n/i18n.h"

namespace ADS::Core {
    class App {
    private:
        /**
         * Environment class to manage the .env content.
         */
        Environment* environment;

        /**
         * Available translations for the app
         */
        i18n::i18n translations;

        /**
         * Imgui object to interact with the GUI
         */
        UI::ImGuiManager imguiObject;

        /**
         * Init all structures that the App class need.
         */
        void init();

    public:
        /**
         * Class Constructor
         */
        App();

        /**
         * Class Destructor
         */
        ~App();

        /**
         * Return the environment for the whole App
         *
         * @return Environment
         */
        ADS::Environment* getEnv() const;

        /**
         * Delegate in Environment::isDebug function to return a boolean
         * indicating is debug mode is on or not.
         *
         * @return bool
         */
        bool isDebug();

        /**
         * Get the logger instance for app-wide usage
         *
         * @return Shared pointer to the spdlog logger
         */
        static std::shared_ptr<spdlog::logger> getLogger();

        /**
         * Get the translations instance for app-wide usage
         *
         * @return Reference to the i18n translations instance
         */
        i18n::i18n& getTranslations();

        /**
         * Return the translation for the text in language given
         *
         * @param text
         * @param language
         *
         * @return
         */
        string _t(const string& text, const string& language) const;

        [[nodiscard]] Environment* getEnvironment();
        [[nodiscard]] i18n::i18n& getTranslatons();
        [[nodiscard]] UI::ImGuiManager& getImGuiObject();
    };
}

#endif //ADS_APP_H