/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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
 * @file TranslationCatalogTests.cpp
 * @brief Coverage for Core::TranslationCatalog id scheme + get/set routing.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

#include "Core/Project.h"
#include "Core/TranslationCatalog.h"

using ADS::Core::Project;
using ADS::Core::TranslationCatalog;
using ADS::Core::TranslationField;
using ADS::Core::TranslationGroup;
using ADS::Types::CharacterId;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;
using ADS::Types::StateId;

namespace {

    /// Two-language project (default en_US=21, plus es_ES=7) with one of each entity.
    std::unique_ptr<Project> makeProject()
    {
        auto p = std::make_unique<Project>("Demo");
        p->getGameData().setLanguages({21, {21, 7}});
        p->addScene(SceneId(1), "entrance");
        p->addState(StateId(2), "locked");
        p->addCharacter(CharacterId(3), "hero");
        p->addItem(ObjectId(4), "key");
        return p;
    }

} // namespace

TEST(TranslationCatalog, MakeId_Composes)
{
    EXPECT_EQ(TranslationCatalog::makeId(TranslationGroup::Scenes, 1, TranslationField::Name),
              "scene.1.name");
    EXPECT_EQ(TranslationCatalog::makeId(TranslationGroup::Items, 4, TranslationField::DescLong),
              "item.4.desc.long");
    EXPECT_EQ(TranslationCatalog::makeId(TranslationGroup::Characters, 3, TranslationField::DescSound),
              "character.3.desc.sound");
}

TEST(TranslationCatalog, Enumerate_YieldsFiveRowsPerEntityInOrder)
{
    const auto p = makeProject();
    const auto rows = TranslationCatalog::enumerate(*p);

    ASSERT_EQ(rows.size(), 4u * 5u); // 4 entities x (name + 4 desc slots)

    // First five rows belong to the scene, id-ordered groups first.
    EXPECT_EQ(rows[0].id, "scene.1.name");
    EXPECT_EQ(rows[0].group, TranslationGroup::Scenes);
    EXPECT_EQ(rows[0].entityLabel, "entrance");
    EXPECT_EQ(rows[1].id, "scene.1.desc.normal");
    EXPECT_EQ(rows[4].id, "scene.1.desc.sound");

    EXPECT_EQ(rows[5].id, "state.2.name");
    EXPECT_EQ(rows[10].id, "character.3.name");
    EXPECT_EQ(rows[15].id, "item.4.name");
}

TEST(TranslationCatalog, Get_DefaultLanguageName_FallsBackToStructuralName)
{
    const auto p = makeProject();
    // en_US is the project default; no override set -> structural name.
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.name", "en_US"), "entrance");
    // A non-default language with no override -> empty.
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.name", "es_ES"), "");
}

TEST(TranslationCatalog, SetName_DefaultLanguage_UpdatesStructuralName)
{
    auto p = makeProject();
    TranslationCatalog::set(*p, "scene.1.name", "en_US", "The Entrance");

    EXPECT_EQ(p->getScenes()[0]->getDisplayName(), "The Entrance");
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.name", "en_US"), "The Entrance");
}

TEST(TranslationCatalog, SetName_NonDefaultLanguage_DoesNotTouchStructuralName)
{
    auto p = makeProject();
    TranslationCatalog::set(*p, "scene.1.name", "es_ES", "La entrada");

    EXPECT_EQ(p->getScenes()[0]->getDisplayName(), "entrance"); // unchanged
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.name", "es_ES"), "La entrada");
}

TEST(TranslationCatalog, SetDescription_RoutesToTheRightSlotAndLanguage)
{
    auto p = makeProject();
    TranslationCatalog::set(*p, "item.4.desc.normal", "es_ES", "Una llave oxidada.");
    TranslationCatalog::set(*p, "item.4.desc.odor",   "en_US", "Rust.");

    EXPECT_EQ(TranslationCatalog::get(*p, "item.4.desc.normal", "es_ES"), "Una llave oxidada.");
    EXPECT_EQ(TranslationCatalog::get(*p, "item.4.desc.odor",   "en_US"), "Rust.");
    EXPECT_EQ(TranslationCatalog::get(*p, "item.4.desc.normal", "en_US"), ""); // other lang untouched
    EXPECT_EQ(TranslationCatalog::get(*p, "item.4.desc.long",   "es_ES"), ""); // other slot untouched
}

TEST(TranslationCatalog, Set_EmptyText_ClearsTheEntry)
{
    auto p = makeProject();
    TranslationCatalog::set(*p, "character.3.desc.sound", "es_ES", "Pasos.");
    ASSERT_EQ(TranslationCatalog::get(*p, "character.3.desc.sound", "es_ES"), "Pasos.");

    TranslationCatalog::set(*p, "character.3.desc.sound", "es_ES", "");
    EXPECT_EQ(TranslationCatalog::get(*p, "character.3.desc.sound", "es_ES"), "");
}

TEST(TranslationCatalog, Get_UnknownIdOrEntity_ReturnsEmpty)
{
    const auto p = makeProject();
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.99.name", "en_US"), "");
    EXPECT_EQ(TranslationCatalog::get(*p, "garbage", "en_US"), "");
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.desc.unknown", "en_US"), "");
}
