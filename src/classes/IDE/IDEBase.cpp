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


#include "IDEBase.h"

#include "app.h"

namespace ADS::IDE {

    IDEBase::IDEBase()
    {
        this->m_environment = Core::App::getEnv();
        this->m_translationsManager = Core::App::getTranslationsManager();
    }

    i18n::i18n * IDEBase::getTranslationManager()
    {
        return this->m_translationsManager;
    }

    Environment * IDEBase::getEnvironment()
    {
        return this->m_environment;
    }


} // ADS
