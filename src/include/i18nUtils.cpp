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

#include <ostream>
#include <string>
#include <vector>

#include "languages.h"
#include "i18n/i18n.h"

namespace lang = ADS::Constants::Languages;
namespace i18n = ADS::i18n;

i18n::i18n loadI18n(const std::vector<std::string>& languages)
{
    std::string baseTranslationFolder = "public/translations/core";

    // Create a new translations with English as fallback.
    i18n::i18n translations = i18n::i18n(baseTranslationFolder, std::string(ADS::Constants::Languages::ENGLISH_UNITED_STATES));

    for (const auto& language : languages) {
        translations.addLanguage(std::string(language));
    }

    auto x = translations.getTranslationStats();
    std::cout << "Translations saved successfully" << std::endl;

    return translations;
}