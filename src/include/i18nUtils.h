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


#ifndef ADS_I18NUTILS_H
#define ADS_I18NUTILS_H

#include "i18n/i18n.h"

namespace lang = ADS::Constants::Languages;
namespace i18n = ADS::i18n;

i18n::i18n loadI18n(const std::vector<std::string>& languages);

#endif //ADS_I18NUTILS_H