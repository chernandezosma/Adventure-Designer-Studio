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
#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H
#include <map>


namespace ADS::Classes::Translations {

    class Translations {

        using translations = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;


        enum class Key { One, Two };

        int main() {

            const std::map<Key, const char *> englishLan{{Key::One, "one"},
                                                         {Key::Two, "two"}};
            std::map<Key, const char *> lan = englishLan;

        /**
         * language and country identifier
         */
        string locale;

        /**
         * Fallback language
         */
        string fallback;

        /**
         * Extract the lang and country from locale
         *
         * @param localeInputString
         * @return
         */
        std::string extractLangFromLocale(const std::optional<std::string> &localeInputString) const;

    public:
        explicit Translations(const string &fallback = SPANISH_SPAIN);
        ~Translations();

        string getLanguageFromLocale(const std::string &locale);

        [[nodiscard]] bool exists(const std::string &language) const;

        string getLocale() const;
        Translations *setLocale(const string &locale);

        [[nodiscard]] string getFallback() const;
        Translations *setFallback(const std::string &locale);

        i18n::translations getTranslations() const;
        void setTranslations(const i18n::translations& translations);

        Translations* loadFromFile();

    };
};

#endif //TRANSLATIONS_H
