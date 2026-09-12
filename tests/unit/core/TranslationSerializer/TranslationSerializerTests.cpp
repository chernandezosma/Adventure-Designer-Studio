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
 * @file TranslationSerializerTests.cpp
 * @brief Round-trip and error-path coverage for Core::TranslationSerializer.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>

#include "Core/Project.h"
#include "Core/TranslationCatalog.h"
#include "Core/TranslationSerializer.h"

namespace fs = std::filesystem;

using ADS::Core::Project;
using ADS::Core::TranslationCatalog;
using ADS::Core::TranslationSerializer;
using ADS::Core::TranslationSet;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;

namespace {

    class TranslationSerializerTest : public ::testing::Test {
    protected:
        fs::path dir;

        void SetUp() override
        {
            dir = fs::temp_directory_path() /
                  ("ads_trn_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
            fs::remove_all(dir);
            fs::create_directories(dir);
        }

        void TearDown() override
        {
            std::error_code ec;
            fs::remove_all(dir, ec);
        }
    };

    std::unique_ptr<Project> makeProject()
    {
        auto p = std::make_unique<Project>("Demo");
        p->getGameData().setLanguages({21, {21, 7}}); // en_US default, + es_ES
        p->addScene(SceneId(1), "entrance");
        p->addItem(ObjectId(2), "key");
        TranslationCatalog::set(*p, "scene.1.name", "es_ES", "La entrada");
        TranslationCatalog::set(*p, "scene.1.desc.normal", "es_ES", "Un umbral de piedra.");
        TranslationCatalog::set(*p, "item.2.desc.odor", "en_US", "Rust.");
        return p;
    }

} // namespace

TEST_F(TranslationSerializerTest, CollectApply_RoundTrips)
{
    const auto source = makeProject();
    const TranslationSet set = TranslationSerializer::collect(*source);

    // The default-language name always appears; overrides appear under their lang.
    EXPECT_EQ(set.at("en_US").at("scene.1.name"), "entrance");
    EXPECT_EQ(set.at("es_ES").at("scene.1.name"), "La entrada");
    EXPECT_EQ(set.at("es_ES").at("scene.1.desc.normal"), "Un umbral de piedra.");
    EXPECT_EQ(set.at("en_US").at("item.2.desc.odor"), "Rust.");

    // Apply into a fresh project with the same structure.
    auto target = std::make_unique<Project>("Demo");
    target->getGameData().setLanguages({21, {21, 7}});
    target->addScene(SceneId(1), "entrance");
    target->addItem(ObjectId(2), "key");
    TranslationSerializer::apply(*target, set);

    EXPECT_EQ(TranslationCatalog::get(*target, "scene.1.name", "es_ES"), "La entrada");
    EXPECT_EQ(TranslationCatalog::get(*target, "scene.1.desc.normal", "es_ES"), "Un umbral de piedra.");
    EXPECT_EQ(TranslationCatalog::get(*target, "item.2.desc.odor", "en_US"), "Rust.");
}

TEST_F(TranslationSerializerTest, Collect_DropsLanguagesRemovedFromSupportedSet)
{
    auto p = makeProject(); // es_ES holds text
    ASSERT_FALSE(TranslationSerializer::collect(*p).count("es_ES") == 0);

    // Remove es_ES from the project's supported set — the DataObject still
    // carries the es_ES text, but collect() must no longer surface it.
    p->getGameData().setLanguages({21, {21}});

    const TranslationSet set = TranslationSerializer::collect(*p);
    EXPECT_EQ(set.count("es_ES"), 0u);
    EXPECT_EQ(set.at("en_US").at("scene.1.name"), "entrance"); // default still there
}

TEST_F(TranslationSerializerTest, ToJson_HasFormatAndSchemaVersion)
{
    const auto p = makeProject();
    const nlohmann::json doc = TranslationSerializer::toJson(TranslationSerializer::collect(*p));

    EXPECT_EQ(doc.at("format"), "ads-translations");
    EXPECT_EQ(doc.at("schemaVersion"), TranslationSerializer::kSchemaVersion);
    EXPECT_TRUE(doc.at("languages").contains("es_ES"));
}

TEST_F(TranslationSerializerTest, FromJson_SchemaMismatch_ReturnsEmpty)
{
    nlohmann::json doc;
    doc["format"] = "ads-translations";
    doc["schemaVersion"] = 99;
    doc["languages"]["es_ES"]["scene.1.name"] = "x";

    EXPECT_TRUE(TranslationSerializer::fromJson(doc).empty());
}

TEST_F(TranslationSerializerTest, SaveLoadFile_RoundTrips)
{
    const auto source = makeProject();
    const fs::path trn = dir / "demo.trn";
    ASSERT_TRUE(TranslationSerializer::saveToFile(*source, trn));
    ASSERT_TRUE(fs::exists(trn));

    auto target = std::make_unique<Project>("Demo");
    target->getGameData().setLanguages({21, {21, 7}});
    target->addScene(SceneId(1), "entrance");
    target->addItem(ObjectId(2), "key");

    ASSERT_TRUE(TranslationSerializer::loadFromFile(*target, trn));
    EXPECT_EQ(TranslationCatalog::get(*target, "scene.1.name", "es_ES"), "La entrada");
    EXPECT_EQ(TranslationCatalog::get(*target, "item.2.desc.odor", "en_US"), "Rust.");
}

TEST_F(TranslationSerializerTest, LoadFile_Missing_IsNotAnError)
{
    auto p = makeProject();
    EXPECT_TRUE(TranslationSerializer::loadFromFile(*p, dir / "no-such-file.trn"));
    // Project untouched.
    EXPECT_EQ(TranslationCatalog::get(*p, "scene.1.name", "es_ES"), "La entrada");
}

TEST_F(TranslationSerializerTest, LoadFile_Malformed_ReturnsFalse)
{
    const fs::path bad = dir / "bad.trn";
    std::ofstream(bad) << "{ not json ";

    auto p = makeProject();
    EXPECT_FALSE(TranslationSerializer::loadFromFile(*p, bad));
}
