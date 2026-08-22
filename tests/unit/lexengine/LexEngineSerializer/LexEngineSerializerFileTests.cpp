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

#include "LexEngineSerializerFileTests.h"

#include <fstream>

#include "LexEngine/LexEngineSerializer.h"

using namespace ADS::LexEngine;

namespace fs = std::filesystem;

void LexEngineSerializerFileTests::SetUp()
{
    testDir = fs::temp_directory_path() / fs::path("ads_lexengine_tests_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
    fs::remove_all(testDir);
    fs::create_directories(testDir);
}

void LexEngineSerializerFileTests::TearDown()
{
    fs::remove_all(testDir);
}

namespace {

    LexEntry makeEntry(LexEntryId id, const std::string& lang, const std::string& canonical) {
        LexEntry entry;
        entry.id        = id;
        entry.lang       = lang;
        entry.canonical  = canonical;
        entry.accumulate(1);
        return entry;
    }

} // namespace

TEST_F(LexEngineSerializerFileTests, SaveThenLoad_RoundTripPreservesAllEntries)
{
    LexEngine original;
    original.restoreEntry(makeEntry(1, "en_US", "key"), "en_US");
    original.restoreEntry(makeEntry(2, "es_ES", "llave"), "es_ES");

    const fs::path path = testDir / "vocabulary.json";
    ASSERT_TRUE(LexEngineSerializer::saveToFile(original, path));
    ASSERT_TRUE(fs::exists(path));

    LexEngine restored;
    ASSERT_TRUE(LexEngineSerializer::loadFromFile(restored, path));

    EXPECT_EQ(restored.findById(1)->canonical, "key");
    EXPECT_EQ(restored.findById(2)->canonical, "llave");
}

TEST_F(LexEngineSerializerFileTests, LoadFromFile_MissingFile_ReturnsFalse)
{
    LexEngine engine;

    EXPECT_FALSE(LexEngineSerializer::loadFromFile(engine, testDir / "does-not-exist.json"));
}

TEST_F(LexEngineSerializerFileTests, LoadFromFile_MalformedJson_ReturnsFalseWithoutThrowing)
{
    const fs::path path = testDir / "malformed.json";
    std::ofstream file(path);
    file << "{not valid json";
    file.close();

    LexEngine engine;

    EXPECT_NO_THROW({
        EXPECT_FALSE(LexEngineSerializer::loadFromFile(engine, path));
    });
}
