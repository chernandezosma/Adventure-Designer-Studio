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

/**
 * @file EntityTranslationEnvironment.cpp
 * @brief Global GTest environment that activates the real i18n translations
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Entities::Scene/Character/Item read display names, category names, and
 * option labels through BaseEntity::translate(), which forwards to
 * ADS::i18n::i18n::getActiveInstance(). The test binary never constructs
 * ADS::Core::App (deliberately — App pulls in SDL/ImGui, which unit tests
 * don't need), so without this environment that active instance stays
 * unset and translate() falls back to returning the raw translation key
 * (e.g. "SCENE.PROP_NAME" instead of "Name"). Registering a real i18n
 * instance here — loaded from the project's actual en_US.json, via the
 * PROJECT_ROOT compile definition set in the top-level CMakeLists.txt —
 * keeps entity unit tests exercising the real translated strings.
 */

#include <gtest/gtest.h>

#include <memory>

#include "i18n/i18n.h"

namespace {
    class EntityTranslationEnvironment final : public ::testing::Environment {
    public:
        void SetUp() override {
            m_i18n = std::make_unique<ADS::i18n::i18n>(
                std::string(PROJECT_ROOT) + "/public/translations/core",
                "en_US"
            );
            ADS::i18n::i18n::setActiveInstance(m_i18n.get());
        }

        void TearDown() override {
            ADS::i18n::i18n::setActiveInstance(nullptr);
            m_i18n.reset();
        }

    private:
        std::unique_ptr<ADS::i18n::i18n> m_i18n;
    };

    ::testing::Environment* const registered =
        ::testing::AddGlobalTestEnvironment(new EntityTranslationEnvironment());
}
