/********************************************************************************
 * Project Name:    Adventure Designer Studio
 * Filename:        translations.h
 * Description:     Manage the translations to be used in game
 *
 * Crafted with passion and love in Canary Islands.
 *
 * Copyright (C) 2024 El Estado Web Research & Development. All rights reserved.
 *
 * This document and its contents, including but not limited to code, schemas,
 * images and diagrams are protected by copyright law in Spain and other countries.
 * Unauthorized use, reproduction, distribution, modification, public display, or
 * public performance of any portion of this material is strictly prohibited.
 *
 * Contact Information
 *
 * For permission to use any portion of this material, please contact at
 * El Estado Web Research & Development, S.L. <info@elestadoweb.com>
 ********************************************************************************/
#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H
#include <string>

#include <i18n_keyval/i18n.hpp>
#include <i18n_keyval/translators/basic.hpp>

#include "../../constants/languages.h"

namespace ADS::Classes::Translations {
    using std::string;
    using namespace i18n::literals;
    using ADS::Constants::Languages::languages;

    class Translations {
        /**
         * language and country identifier
         */
        string locale;

        /**
         * Fallback language
         */
        string fallback;

        /**
         * Base folder for all Translations
         */
        string basePath;

        /**
         * Translations strings
         */
        i18n::translations translations;

        /**
         * Extract the lang and country from locale
         *
         * @param localeInputString
         * @return
         */
        std::string extractLangFromLocale(const std::optional<std::string> &localeInputString) const;

    public:
        explicit Translations(const string &baseFolder, const string &fallback = SPANISH_SPAIN);
        ~Translations();

        // loadTranslationsFromFile()

        string getLanguageFromLocale(const std::string &locale);

        [[nodiscard]] bool exists(const std::string &language) const;

        string getLocale() const;
        Translations *setLocale(const string &locale);

        [[nodiscard]] string getFallback() const;
        Translations *setFallback(const std::string &locale);

        i18n::translations getTranslations() const;
        void setTranslations(const i18n::translations& translations);
    };
};

#endif //TRANSLATIONS_H
