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
#include <fstream>
#include <iostream>

#include "languages.h"
#include "i18n/i18n.h"

int main() {
    std::cout << "Start of Main" << std::endl;
    std::cout << "Using the C++ version: " << __cplusplus << std::endl;;

    // ADS::i18n::i18n translations = ADS::i18n::i18n(
    //     "translations",
    //     ADS::Constants::Languages::SPANISH_SPAIN.data()
    // );
    //
    // std::string language = translations.getLocaleLanguage();
    // translations.addLanguage(ADS::Constants::Languages::ENGLISH_UNITED_KINGDOM.data());
    //
    // auto uk = translations.getLanguage(ADS::Constants::Languages::ENGLISH_UNITED_KINGDOM.data());
    // auto es = translations.getLanguage(translations.getFallbackLanguage());
    //
    // translations.addTranslation("HELLO", "Hi!", "Hola");
    // translations.addTranslation("BYE", "see you", "Adios");

    // translations.addTranslation(
    //     ADS::Constants::Languages::SPANISH_SPAIN.data(),
    //     "BYE",
    //     "Adios"
    // );

    // translations.addTranslation(
    //     ADS::Constants::Languages::SPANISH_SPAIN.data(),
    //     "BYE",
    //     "Chao!"
    // );

    // translations.addTranslation(
    //     ADS::Constants::Languages::ENGLISH_AUSTRALIA.data(),
    //     "HOUSE",
    //     "Jouse"
    // );

    std::cout << "End of Main" << std::endl;
    return 0;
}
