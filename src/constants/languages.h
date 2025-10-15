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
#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace ADS::Constants::Languages {
    // POSIX locale codes (standard format)
    constexpr std::string_view SPANISH_ARGENTINA = "es_AR";
    constexpr std::string_view SPANISH_BOLIVIA = "es_BO";
    constexpr std::string_view SPANISH_CHILE = "es_CL";
    constexpr std::string_view SPANISH_COLOMBIA = "es_CO";
    constexpr std::string_view SPANISH_COSTA_RICA = "es_CR";
    constexpr std::string_view SPANISH_DOMINICAN_REPUBLIC = "es_DO";
    constexpr std::string_view SPANISH_ECUADOR = "es_EC";
    constexpr std::string_view SPANISH_SPAIN = "es_ES";
    constexpr std::string_view SPANISH_GUATEMALA = "es_GT";
    constexpr std::string_view SPANISH_HONDURAS = "es_HN";
    constexpr std::string_view SPANISH_MEXICO = "es_MX";
    constexpr std::string_view SPANISH_NICARAGUA = "es_NI";
    constexpr std::string_view SPANISH_PANAMA = "es_PA";
    constexpr std::string_view SPANISH_PERU = "es_PE";
    constexpr std::string_view SPANISH_PUERTO_RICO = "es_PR";
    constexpr std::string_view SPANISH_PARAGUAY = "es_PY";
    constexpr std::string_view SPANISH_EL_SALVADOR = "es_SV";
    constexpr std::string_view SPANISH_UNITED_STATES = "es_US";
    constexpr std::string_view SPANISH_URUGUAY = "es_UY";
    constexpr std::string_view SPANISH_VENEZUELA = "es_VE";

    constexpr std::string_view ENGLISH_UNITED_KINGDOM = "en_GB"; // Changed from en_UK to standard en_GB
    constexpr std::string_view ENGLISH_UNITED_STATES = "en_US";
    constexpr std::string_view ENGLISH_CANADA = "en_CA";
    constexpr std::string_view ENGLISH_AUSTRALIA = "en_AU";

    constexpr std::string_view FRENCH_BELGIUM = "fr_BE";
    constexpr std::string_view FRENCH_CANADA = "fr_CA";
    constexpr std::string_view FRENCH_SWITZERLAND = "fr_CH";
    constexpr std::string_view FRENCH_FRANCE = "fr_FR";
    constexpr std::string_view FRENCH_LUXEMBOURG = "fr_LU";
    constexpr std::string_view FRENCH_MONACO = "fr_MC";

    constexpr std::string_view GERMAN_AUSTRIA = "de_AT";
    constexpr std::string_view GERMAN_SWITZERLAND = "de_CH";
    constexpr std::string_view GERMAN_GERMANY = "de_DE";
    constexpr std::string_view GERMAN_LUXEMBOURG = "de_LU";

    constexpr std::string_view DUTCH_BELGIUM = "nl_BE";
    constexpr std::string_view DUTCH_NETHERLANDS = "nl_NL";
    constexpr std::string_view ITALIAN_ITALY = "it_IT";
    constexpr std::string_view ITALIAN_SWITZERLAND = "it_CH";
    constexpr std::string_view PORTUGUESE_BRAZIL = "pt_BR";
    constexpr std::string_view PORTUGUESE_PORTUGAL = "pt_PT";
    constexpr std::string_view SWEDISH_FINLAND = "sv_FI";
    constexpr std::string_view SWEDISH_SWEDEN = "sv_SE";
    constexpr std::string_view POLISH_POLAND = "pl_PL";
    constexpr std::string_view RUSSIAN_RUSSIA = "ru_RU";
    constexpr std::string_view ROMANIAN_ROMANIA = "ro_RO";

    constexpr std::string_view DEFAULT_FALLBACK = "es_ES";

    // WIN32 platform locale strings
#ifdef _WIN32
    namespace Win32 {
        constexpr std::string_view SPANISH_ARGENTINA = "Spanish_Argentina";
        constexpr std::string_view SPANISH_BOLIVIA = "Spanish_Bolivia";
        constexpr std::string_view SPANISH_CHILE = "Spanish_Chile";
        constexpr std::string_view SPANISH_COLOMBIA = "Spanish_Colombia";
        constexpr std::string_view SPANISH_COSTA_RICA = "Spanish_Costa Rica";
        constexpr std::string_view SPANISH_DOMINICAN_REPUBLIC = "Spanish_Dominican Republic";
        constexpr std::string_view SPANISH_ECUADOR = "Spanish_Ecuador";
        constexpr std::string_view SPANISH_SPAIN = "Spanish_Spain";
        constexpr std::string_view SPANISH_GUATEMALA = "Spanish_Guatemala";
        constexpr std::string_view SPANISH_HONDURAS = "Spanish_Honduras";
        constexpr std::string_view SPANISH_MEXICO = "Spanish_Mexico";
        constexpr std::string_view SPANISH_NICARAGUA = "Spanish_Nicaragua";
        constexpr std::string_view SPANISH_PANAMA = "Spanish_Panama";
        constexpr std::string_view SPANISH_PERU = "Spanish_Peru";
        constexpr std::string_view SPANISH_PUERTO_RICO = "Spanish_Puerto Rico";
        constexpr std::string_view SPANISH_PARAGUAY = "Spanish_Paraguay";
        constexpr std::string_view SPANISH_EL_SALVADOR = "Spanish_El Salvador";
        constexpr std::string_view SPANISH_UNITED_STATES = "Spanish_United States";
        constexpr std::string_view SPANISH_URUGUAY = "Spanish_Uruguay";
        constexpr std::string_view SPANISH_VENEZUELA = "Spanish_Venezuela";

        constexpr std::string_view ENGLISH_UNITED_KINGDOM = "English_United Kingdom";
        constexpr std::string_view ENGLISH_UNITED_STATES = "English_United States";
        constexpr std::string_view ENGLISH_CANADA = "English_Canada";
        constexpr std::string_view ENGLISH_AUSTRALIA = "English_Australia";

        constexpr std::string_view FRENCH_BELGIUM = "French_Belgium";
        constexpr std::string_view FRENCH_CANADA = "French_Canada";
        constexpr std::string_view FRENCH_SWITZERLAND = "French_Switzerland";
        constexpr std::string_view FRENCH_FRANCE = "French_France";
        constexpr std::string_view FRENCH_LUXEMBOURG = "French_Luxembourg";
        constexpr std::string_view FRENCH_MONACO = "French_Monaco";

        constexpr std::string_view GERMAN_AUSTRIA = "German_Austria";
        constexpr std::string_view GERMAN_SWITZERLAND = "German_Switzerland";
        constexpr std::string_view GERMAN_GERMANY = "German_Germany";
        constexpr std::string_view GERMAN_LUXEMBOURG = "German_Luxembourg";

        constexpr std::string_view DUTCH_BELGIUM = "Dutch_Belgium";
        constexpr std::string_view DUTCH_NETHERLANDS = "Dutch_Netherlands";
        constexpr std::string_view ITALIAN_ITALY = "Italian_Italy";
        constexpr std::string_view ITALIAN_SWITZERLAND = "Italian_Switzerland";
        constexpr std::string_view PORTUGUESE_BRAZIL = "Portuguese_Brazil";
        constexpr std::string_view PORTUGUESE_PORTUGAL = "Portuguese_Portugal";
        constexpr std::string_view SWEDISH_FINLAND = "Swedish_Finland";
        constexpr std::string_view SWEDISH_SWEDEN = "Swedish_Sweden";
        constexpr std::string_view POLISH_POLAND = "Polish_Poland";
        constexpr std::string_view RUSSIAN_RUSSIA = "Russian_Russia";
        constexpr std::string_view ROMANIAN_ROMANIA = "Romanian_Romania";
    }
#endif

    constexpr auto languages =
            std::to_array<std::pair<std::string_view, std::string_view> >({
                {SPANISH_ARGENTINA, "Spanish (Argentina)"},
                {SPANISH_BOLIVIA, "Spanish (Bolivia)"},
                {SPANISH_CHILE, "Spanish (Chile)"},
                {SPANISH_COLOMBIA, "Spanish (Colombia)"},
                {SPANISH_COSTA_RICA, "Spanish (Costa Rica)"},
                {SPANISH_DOMINICAN_REPUBLIC, "Spanish (Dominican Republic)"},
                {SPANISH_ECUADOR, "Spanish (Ecuador)"},
                {SPANISH_SPAIN, "Spanish (Spain)"},
                {SPANISH_GUATEMALA, "Spanish (Guatemala)"},
                {SPANISH_HONDURAS, "Spanish (Honduras)"},
                {SPANISH_MEXICO, "Spanish (Mexico)"},
                {SPANISH_NICARAGUA, "Spanish (Nicaragua)"},
                {SPANISH_PANAMA, "Spanish (Panama)"},
                {SPANISH_PERU, "Spanish (Peru)"},
                {SPANISH_PUERTO_RICO, "Spanish (Puerto Rico)"},
                {SPANISH_PARAGUAY, "Spanish (Paraguay)"},
                {SPANISH_EL_SALVADOR, "Spanish (El Salvador)"},
                {SPANISH_UNITED_STATES, "Spanish (United States)"},
                {SPANISH_URUGUAY, "Spanish (Uruguay)"},
                {SPANISH_VENEZUELA, "Spanish (Venezuela)"},

                {ENGLISH_UNITED_KINGDOM, "English (United Kingdom)"},
                {ENGLISH_UNITED_STATES, "English (United States)"},
                {ENGLISH_CANADA, "English (Canada)"},
                {ENGLISH_AUSTRALIA, "English (Australia)"},

                {FRENCH_BELGIUM, "French (Belgium)"},
                {FRENCH_CANADA, "French (Canada)"},
                {FRENCH_SWITZERLAND, "French (Switzerland)"},
                {FRENCH_FRANCE, "French (France)"},
                {FRENCH_LUXEMBOURG, "French (Luxembourg)"},
                {FRENCH_MONACO, "French (Monaco)"},

                {GERMAN_AUSTRIA, "German (Austria)"},
                {GERMAN_SWITZERLAND, "German (Switzerland)"},
                {GERMAN_GERMANY, "German (Germany)"},
                {GERMAN_LUXEMBOURG, "German (Luxembourg)"},

                {DUTCH_BELGIUM, "Dutch (Belgium)"},
                {DUTCH_NETHERLANDS, "Dutch (Netherlands)"},
                {ITALIAN_ITALY, "Italian (Italy)"},
                {ITALIAN_SWITZERLAND, "Italian (Switzerland)"},
                {PORTUGUESE_BRAZIL, "Portuguese (Brazil)"},
                {PORTUGUESE_PORTUGAL, "Portuguese (Portugal)"},
                {SWEDISH_FINLAND, "Swedish (Finland)"},
                {SWEDISH_SWEDEN, "Swedish (Sweden)"},
                {POLISH_POLAND, "Polish (Poland)"},
                {RUSSIAN_RUSSIA, "Russian (Russia)"},
                {ROMANIAN_ROMANIA, "Romanian (Romania)"},
            });

#ifdef _WIN32
    // WIN32 to POSIX locale mapping
    const std::unordered_map<std::string_view, std::string_view> win32ToPosixMap = {
        // Spanish
        {Win32::SPANISH_ARGENTINA, SPANISH_ARGENTINA},
        {Win32::SPANISH_BOLIVIA, SPANISH_BOLIVIA},
        {Win32::SPANISH_CHILE, SPANISH_CHILE},
        {Win32::SPANISH_COLOMBIA, SPANISH_COLOMBIA},
        {Win32::SPANISH_COSTA_RICA, SPANISH_COSTA_RICA},
        {Win32::SPANISH_DOMINICAN_REPUBLIC, SPANISH_DOMINICAN_REPUBLIC},
        {Win32::SPANISH_ECUADOR, SPANISH_ECUADOR},
        {Win32::SPANISH_SPAIN, SPANISH_SPAIN},
        {Win32::SPANISH_GUATEMALA, SPANISH_GUATEMALA},
        {Win32::SPANISH_HONDURAS, SPANISH_HONDURAS},
        {Win32::SPANISH_MEXICO, SPANISH_MEXICO},
        {Win32::SPANISH_NICARAGUA, SPANISH_NICARAGUA},
        {Win32::SPANISH_PANAMA, SPANISH_PANAMA},
        {Win32::SPANISH_PERU, SPANISH_PERU},
        {Win32::SPANISH_PUERTO_RICO, SPANISH_PUERTO_RICO},
        {Win32::SPANISH_PARAGUAY, SPANISH_PARAGUAY},
        {Win32::SPANISH_EL_SALVADOR, SPANISH_EL_SALVADOR},
        {Win32::SPANISH_UNITED_STATES, SPANISH_UNITED_STATES},
        {Win32::SPANISH_URUGUAY, SPANISH_URUGUAY},
        {Win32::SPANISH_VENEZUELA, SPANISH_VENEZUELA},

        // English
        {Win32::ENGLISH_UNITED_KINGDOM, ENGLISH_UNITED_KINGDOM},
        {Win32::ENGLISH_UNITED_STATES, ENGLISH_UNITED_STATES},
        {Win32::ENGLISH_CANADA, ENGLISH_CANADA},
        {Win32::ENGLISH_AUSTRALIA, ENGLISH_AUSTRALIA},

        // French
        {Win32::FRENCH_BELGIUM, FRENCH_BELGIUM},
        {Win32::FRENCH_CANADA, FRENCH_CANADA},
        {Win32::FRENCH_SWITZERLAND, FRENCH_SWITZERLAND},
        {Win32::FRENCH_FRANCE, FRENCH_FRANCE},
        {Win32::FRENCH_LUXEMBOURG, FRENCH_LUXEMBOURG},
        {Win32::FRENCH_MONACO, FRENCH_MONACO},

        // German
        {Win32::GERMAN_AUSTRIA, GERMAN_AUSTRIA},
        {Win32::GERMAN_SWITZERLAND, GERMAN_SWITZERLAND},
        {Win32::GERMAN_GERMANY, GERMAN_GERMANY},
        {Win32::GERMAN_LUXEMBOURG, GERMAN_LUXEMBOURG},

        // Dutch
        {Win32::DUTCH_BELGIUM, DUTCH_BELGIUM},
        {Win32::DUTCH_NETHERLANDS, DUTCH_NETHERLANDS},

        // Italian
        {Win32::ITALIAN_ITALY, ITALIAN_ITALY},
        {Win32::ITALIAN_SWITZERLAND, ITALIAN_SWITZERLAND},

        // Portuguese
        {Win32::PORTUGUESE_BRAZIL, PORTUGUESE_BRAZIL},
        {Win32::PORTUGUESE_PORTUGAL, PORTUGUESE_PORTUGAL},

        // Swedish
        {Win32::SWEDISH_FINLAND, SWEDISH_FINLAND},
        {Win32::SWEDISH_SWEDEN, SWEDISH_SWEDEN},

        // Other
        {Win32::POLISH_POLAND, POLISH_POLAND},
        {Win32::RUSSIAN_RUSSIA, RUSSIAN_RUSSIA},
        {Win32::ROMANIAN_ROMANIA, ROMANIAN_ROMANIA}
    };
#endif

    /**
     * @brief Converts platform-specific locale string to standard POSIX format
     *
     * @param platformLocale The platform-specific locale string
     *
     * @return Standard POSIX locale code (e.g., "es_ES") or empty string if not found
     */
    [[nodiscard]] inline std::string normalizePlatformLocale(const std::string_view platformLocale) {
        // Remove codepage/encoding suffixes
        const size_t dotPos = platformLocale.find('.');
        std::string_view baseName = (dotPos != std::string_view::npos)
                                        ? platformLocale.substr(0, dotPos)
                                        : platformLocale;

        // Remove variant suffixes (Unix @variant)
        const size_t atPos = baseName.find('@');
        if (atPos != std::string_view::npos) {
            baseName = baseName.substr(0, atPos);
        }

#ifdef _WIN32
        // Convert WIN32 locale to POSIX
        auto it = win32ToPosixMap.find(baseName);
        if (it != win32ToPosixMap.end()) {
            return std::string(it->second);
        }

        // Fallback: try to extract from common WIN32 patterns
        if (baseName.find("Spanish") == 0) return "es_ES";
        if (baseName.find("English") == 0) return "en_US";
        if (baseName.find("French") == 0) return "fr_FR";
        if (baseName.find("German") == 0) return "de_DE";
        if (baseName.find("Italian") == 0) return "it_IT";
        if (baseName.find("Portuguese") == 0) return "pt_PT";
        if (baseName.find("Dutch") == 0) return "nl_NL";
        if (baseName.find("Polish") == 0) return "pl_PL";
        if (baseName.find("Russian") == 0) return "ru_RU";
        if (baseName.find("Swedish") == 0) return "sv_SE";
        if (baseName.find("Romanian") == 0) return "ro_RO";
#else
        // Unix/Linux/macOS: assume already in POSIX format
        // Just validate it exists in our supported languages
        auto langIt = std::find_if(languages.cbegin(), languages.cend(),
                                   [baseName](const auto &pair) {
                                       return pair.first == baseName;
                                   });

        if (langIt != languages.cend()) {
            return std::string(baseName);
        }

        // Handle some common variations
        if (baseName == "en_UK") {
            return "en_GB";
        }
#endif

        return ""; // Not found
    }

    /**
     * @brief Gets the full language name for a given language code.
     *
     * @param code The language code (e.g., "es_ES") - can be platform-specific
     *
     * @return The full language name or empty string if not found
     */
    [[nodiscard]] inline std::string getLanguageName(const std::string_view code) {
        // First try direct lookup
        auto it = std::find_if(
            languages.cbegin(),
            languages.cend(),
            [code](const auto &pair) {
                return pair.first == code;
            });

        if (it != languages.cend()) {
            return std::string(it->second);
        }

        // Try normalizing the locale first
        std::string normalizedCode = normalizePlatformLocale(code);
        if (!normalizedCode.empty()) {
            auto normalizedIt = std::find_if(languages.cbegin(), languages.cend(),
                                             [normalizedCode](const auto &pair) {
                                                 return pair.first == normalizedCode;
                                             });

            if (normalizedIt != languages.cend()) {
                return std::string(normalizedIt->second);
            }
        }

        return {}; // Not found
    }

    /**
     * @brief Checks if a language code is supported
     * @param code The language code to check
     * @return true if supported, false otherwise
     */
    [[nodiscard]] inline bool isLanguageSupported(const std::string_view code) {
        return !getLanguageName(code).empty();
    }

    /**
     * @brief Gets all supported locale codes
     * @return Array of all supported POSIX locale codes
     */
    [[nodiscard]] constexpr auto getSupportedLocales() {
        constexpr auto locales = std::to_array<std::string_view>({
            SPANISH_ARGENTINA, SPANISH_BOLIVIA, SPANISH_CHILE, SPANISH_COLOMBIA,
            SPANISH_COSTA_RICA, SPANISH_DOMINICAN_REPUBLIC, SPANISH_ECUADOR, SPANISH_SPAIN,
            SPANISH_GUATEMALA, SPANISH_HONDURAS, SPANISH_MEXICO, SPANISH_NICARAGUA,
            SPANISH_PANAMA, SPANISH_PERU, SPANISH_PUERTO_RICO, SPANISH_PARAGUAY,
            SPANISH_EL_SALVADOR, SPANISH_UNITED_STATES, SPANISH_URUGUAY, SPANISH_VENEZUELA,
            ENGLISH_UNITED_KINGDOM, ENGLISH_UNITED_STATES, ENGLISH_CANADA, ENGLISH_AUSTRALIA,
            FRENCH_BELGIUM, FRENCH_CANADA, FRENCH_SWITZERLAND, FRENCH_FRANCE,
            FRENCH_LUXEMBOURG, FRENCH_MONACO,
            GERMAN_AUSTRIA, GERMAN_SWITZERLAND, GERMAN_GERMANY, GERMAN_LUXEMBOURG,
            DUTCH_BELGIUM, DUTCH_NETHERLANDS, ITALIAN_ITALY, ITALIAN_SWITZERLAND,
            PORTUGUESE_BRAZIL, PORTUGUESE_PORTUGAL, SWEDISH_FINLAND, SWEDISH_SWEDEN,
            POLISH_POLAND, RUSSIAN_RUSSIA, ROMANIAN_ROMANIA
        });
        return locales;
    }

    // Optional stream operator for debugging
    template<typename T>
    std::ostream &operator<<(std::ostream &os, const std::optional<T> &opt) {
        return opt ? (os << *opt) : (os << "nullopt");
    }
}

#endif //LANGUAGES_H
