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


#ifndef ADS_I18NUTILS_H
#define ADS_I18NUTILS_H

#include "i18n/i18n.h"

namespace lang = ADS::Constants::Languages;
namespace i18n = ADS::i18n;

/**
 * @brief Load internationalization system with specified languages
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Initializes the i18n translation system and loads the specified languages
 * from translation files. Currently the function declaration exists but the
 * implementation is commented out in the .cpp file.
 *
 * @param languages Vector of language codes to load
 * @return Initialized i18n instance with loaded translations
 *
 * @note This function is currently not implemented (see i18nUtils.cpp)
 */
i18n::i18n loadI18n(const std::vector<std::string>& languages);

#endif //ADS_I18NUTILS_H