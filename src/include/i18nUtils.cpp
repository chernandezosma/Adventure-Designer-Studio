/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
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