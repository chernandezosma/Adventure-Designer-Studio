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

#include <string>
#include <vector>

#include "languages.h"
#include "i18n/i18n.h"
#include "spdlog/spdlog.h"

namespace lang = ADS::Constants::Languages;
namespace i18n = ADS::i18n;

// i18n::i18n loadI18n(const std::vector<std::string>& languages)
// {
//     // Create a new translations with English as fallback.
//     i18n::i18n translations = i18n::i18n(
//         baseTranslationFolder,
//         std::string(ADS::Constants::Languages::ENGLISH_UNITED_STATES)
//     );
//
//
//
//
//     spdlog::info("Translations saved successfully");
//
//     return translations;
// }