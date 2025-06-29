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
#include "translations.h"

#include <fstream>
#include <iostream>
#include <memory>

#include <i18n_keyval/translators/nlohmann_json.hpp>

#include "path_not_exist_exception.h"

namespace ADS::Classes::Translations {
    using namespace i18n::literals;
    using namespace std;
    using namespace ADS::Constants;
    using namespace ADS::Constants::Languages;

    /**
     * Constructor
     *
     * @param baseFolder
     */
    Translations::Translations(const string &baseFolder, const string &fallback) {

        // baseFolder could be empty string ("")
        this->basePath = baseFolder;
        if (!this->basePath.empty()) {
            i18n::initialize_translator<i18n::translators::nlohmann_json>(this->basePath);
        }

        this->fallback = fallback;
        string tempLocale = this->extractLangFromLocale(optional<string>());
        if (!Languages::languages.contains(tempLocale)) {
            this->locale = this->fallback;
        }
        this->setLocale(tempLocale);
    }

    /**
     * Destructor
     */
    Translations::~Translations() {
        // cout << "Translations destroyed for: " << this->basePath << endl;
    }

    /**
     * Return the translations given
     *
     * @return
     */
    i18n::translations Translations::getTranslations() const {
        return this->translations;
    }

    /**
     * Set the translations for the module.
     *
     * @param translations MAP Array
     */
    void Translations::setTranslations(const i18n::translations& translations) {

        try {
            this->translations = translations;
            i18n::initialize_translator(this->translations);
        } catch (const std::exception& e) {
            std::cerr << "Error initializing translator: " << e.what() << std::endl;
        }

    }

    /**
     * Extract the language and country from the locale string or get the
     * locale if no parameter is given.
     *
     * @param localeInputString string from locale
     *
     * @return std::string
     */
    std::string Translations::extractLangFromLocale(const std::optional<std::string> &localeInputString) const {

        std::string localeStr;

        if (!localeInputString.has_value()) {
            localeStr = std::locale("").name();
        } else {
            const std::string &param_str = localeInputString.value();
            localeStr = (param_str.empty()) ? std::locale("").name() : param_str;
        }

        size_t equalPos = localeStr.find('=');
        size_t dotPos = localeStr.find('.');

        // equalPos = (equalPos == std::string::npos) ? 0 : 1;
        if (dotPos != std::string::npos && dotPos > equalPos) {
            return localeStr.substr(equalPos + 1, dotPos - equalPos - 1);
        }

        return localeStr.substr(equalPos);
    }

    /**
     * Look for the language into ADS::Constants::Languages::language for a language (xx-XX)
     * and return the row if the language is in the map or the end if otherwise
     *
     * @param locale    language_country identification. ie: en_US
     *
     * @return string | null
     */
    string Translations::getLanguageFromLocale(const std::string &locale) {

        string language;

        try {
            language = Languages::languages.at(locale);
        } catch (const std::out_of_range& oor) {
            language =  std::string();
        }

        return language;
    }

    /**
     * Return true if the language is in the languages map, false otherwise
     *
     * @param language    language_country identification. ie: en_US
     *
     * @return bool
     */

    bool Translations::exists(const std::string &language) const {

        bool result = false;

        try {
            result = Languages::languages.contains(language) && this->translations.contains(language);
        } catch (const std::out_of_range& oor) {
            result = false;
        }

        return result;
    }

    /**
     * Return the locale saved into the class
     *
     * @return string
     */
    string Translations::getLocale() const {
        return this->locale;
    }

    /**
     * Set the locale passed as parameter into the class.
     *
     * @param locale    language_country identification. ie: en_US
     *
     * @return Translations*
     */
    Translations *Translations::setLocale(const string &locale) {
        try {
            // look for the language in supported languages, if it cannot be found, setup the fallback;
            if (!locale.empty() && !this->exists(locale) && !this->translations.contains(locale)) {
                if (!this->fallback.empty() && !this->exists(this->fallback) && !this->translations.contains(this->fallback)) {
                    this->fallback = SPANISH_SPAIN;
                }
                this->locale = this->fallback;
            } else {
                this->locale = locale;
            }
            i18n::set_locale(this->locale);
        } catch (const i18n::i18n_exception& e) {
            std::cerr << e.what() << endl;
            return this;
        }

        return this;
    }

    /**
     * Return the language fallback
     *
     * @return string
     */
    string Translations::getFallback() const {
        return this->fallback;
    }

    /**
     * Set the fallback language
     *
     * @param locale    language_country identification. ie: en_US
     *
     * @return Translations*
     */
    Translations* Translations::setFallback(const std::string &locale) {

        if (!locale.empty() && this->exists(locale)) {
            this->fallback = locale;
        }

        return this;
    }

    Translations* Translations::loadFromFile() {
        std::ifstream ifs((string) "../translations/ads/" + this->locale + ".json", std::ifstream::in);
        nlohmann::json jf = nlohmann::json::parse(ifs);
        this->setTranslations(jf);

        return this;
    }
}
