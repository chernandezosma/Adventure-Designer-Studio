/********************************************************************************
 * Project Name:    Adventure Designer Studio
 * Filename:        languages.h
 * Description:     List of language constants that could be used in the app.
 *                  Extracted from IBM Source
 *                  https://www.ibm.com/docs/es/datacap/9.1.9?topic=support-supported-language-codes
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
#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <map>
#include <string>

namespace ADS::Constants::Languages {

    #define SPANISH_ARGENTINA "es_AR"
    #define SPANISH_BOLIVIA "es_BO"
    #define SPANISH_CHILE "es_CL"
    #define SPANISH_COLOMBIA "es_CO"
    #define SPANISH_COSTA_RICA "es_CR"
    #define SPANISH_DOMINICAN_REPUBLIC "es_DO"
    #define SPANISH_ECUADOR "es_EC"
    #define SPANISH_SPAIN "es_ES"
    #define SPANISH_GUATEMALA "es_GT"
    #define SPANISH_HONDURAS "es_HN"
    #define SPANISH_MEXICO "es_MX"
    #define SPANISH_NICARAGUA "es_NI"
    #define SPANISH_PANAMA "es_PA"
    #define SPANISH_PERU "es_PE"
    #define SPANISH_PUERTO_RICO "es_PR"
    #define SPANISH_PARAGUAY "es_PY"
    #define SPANISH_EL_SALVADOR "es_SV"
    #define SPANISH_UNITED_STATES "es_US"
    #define SPANISH_URUGUAY "es_UY"
    #define SPANISH_VENEZUELA "es_VE"

    #define ENGLISH_UNITED_KINGDOM "en_UK"
    #define ENGLISH_UNITED_STATES "en_US"
    #define ENGLISH_CANADA "en_CA"
    #define ENGLISH_AUSTRALIA "en_AU"

    #define FRENCH_BELGIUM "fr_BE"
    #define FRENCH_CANADA "fr_CA"
    #define FRENCH_SWITZERLAND "fr_CH"
    #define FRENCH_FRANCE "fr_FR"
    #define FRENCH_LUXEMBOURG "fr_LU"
    #define FRENCH_MONACO "fr_MC"

    #define GERMAN_AUSTRIA "de_AT"
    #define GERMAN_SWITZERLAND "de_CH"
    #define GERMAN_GERMANY "de_DE"
    #define GERMAN_LUXEMBOURG "de_LU"

    #define DUTCH_BELGIUM "nl_BE"
    #define DUTCH_NETHERLANDS "nl_NL"
    #define ITALIAN_ITALY "it_IT"
    #define ITALIAN_SWITZERLAND "it_CH"
    #define PORTUGUESE_BRAZIL "pt_BR"
    #define PORTUGUESE_PORTUGAL "pt_PT"
    #define SWEDISH_FINLAND "sv_FI"
    #define SWEDISH_SWEDEN "sv_SE"
    #define POLISH_POLAND "pl_PL"
    #define RUSSIAN_RUSSIA "ru_RU"
    #define ROMANIAN_ROMANIA "ro_RO"

    inline std::map<std::string, std::string> languages = {
        {"es_AR", "Spanish (Argentina)"},
        {"es_BO", "Spanish (Bolivia)"},
        {"es_CL", "Spanish (Chile)"},
        {"es_CO", "Spanish (Colombia)"},
        {"es_CR", "Spanish (Costa Rica)"},
        {"es_DO", "Spanish (Dominican Republic)"},
        {"es_EC", "Spanish (Ecuador)"},
        {"es_ES", "Spanish (Spain)"},
        {"es_GT", "Spanish (Guatemala)"},
        {"es_HN", "Spanish (Honduras)"},
        {"es_MX", "Spanish (Mexico)"},
        {"es_NI", "Spanish (Nicaragua)"},
        {"es_PA", "Spanish (Panama)"},
        {"es_PE", "Spanish (Peru)"},
        {"es_PR", "Spanish (Puerto Rico)"},
        {"es_PY", "Spanish (Paraguay)"},
        {"es_SV", "Spanish (El Salvador)"},
        {"es_US", "Spanish (United States)"},
        {"es_UY", "Spanish (Uruguay)"},
        {"es_VE", "Spanish (Venezuela)"},

        {"en_UK", "English (United Kingdom)"},
        {"en_US", "English (United States)"},
        {"en_CA", "English (Canada)"},
        {"en_AU", "English (Australia)"},

        {"fr_BE", "French (Belgium)"},
        {"fr_CA", "French (Canada)"},
        {"fr_CH", "French (Switzerland)"},
        {"fr_FR", "French (France)"},
        {"fr_LU", "French (Luxembourg)"},
        {"fr_MC", "French (Monaco)"},

        {"de_AT", "German (Austria)"},
        {"de_CH", "German (Switzerland)"},
        {"de_DE", "German (Germany)"},
        {"de_LU", "German (Luxembourg)"},

        {"nl_BE", "Dutch (Belgium)"},
        {"nl_NL", "Dutch (Netherlands)"},
        {"it_IT", "Italian (Italy)"},
        {"it_CH", "Italian (Switzerland)"},

        {"pt_BR", "Portuguese (Brazil)"},
        {"pt_PT", "Portuguese (Portugal)"},

        {"sv_FI", "Swedish (Finland)"},
        {"sv_SE", "Swedish (Sweden)"},

        {"pl_PL", "Polish (Poland)"},
        {"ru_RU", "Russian (Russia)"},
        {"ro_RO", "Romanian (Romania)"},
    };
}
#endif //LANGUAGES_H
