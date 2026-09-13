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
 * @file ProjectSerializerTests.cpp
 * @brief Round-trip and error-path coverage for Core::ProjectSerializer.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "Core/Project.h"
#include "Core/ProjectSerializer.h"
#include "Core/TranslationCatalog.h"
#include "exceptions/core/project_serialization_exception.h"
#include "exceptions/filesystem/file_not_found_exception.h"

namespace fs = std::filesystem;

using ADS::Core::LoadResult;
using ADS::Core::Project;
using ADS::Core::ProjectSerializer;
using ADS::Types::CharacterId;
using ADS::Types::ChainId;
using ADS::Types::ObjectId;
using ADS::Types::SceneId;
using ADS::Types::StateId;

namespace {

    class ProjectSerializerTest : public ::testing::Test {
    protected:
        fs::path dir;

        void SetUp() override
        {
            dir = fs::temp_directory_path() /
                  ("ads_ps_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
            fs::remove_all(dir);
            fs::create_directories(dir);
        }

        void TearDown() override
        {
            std::error_code ec;
            fs::remove_all(dir, ec);
        }

        std::string readFile(const fs::path& p) const
        {
            std::ifstream in(p);
            return std::string(std::istreambuf_iterator<char>(in), {});
        }
    };

    /// Build a project exercising most fields of every DataObject.
    std::unique_ptr<Project> makeRichProject()
    {
        // The ctor sets game.title (== the project name).
        auto project = std::make_unique<Project>("The Forgotten Crypt");

        auto& game = project->getGameData();
        game.setSynopsis("A lone adventurer descends into the dark.");
        game.setAuthor({"Ada Lovelace", "ada@example.com"});
        game.setVersion("1.4.2");
        game.setLanguages({7, {7, 21, 27}});
        game.setInventoryCapacity(16);

        for (int i = 1; i <= 5; ++i) {
            project->addScene(SceneId(static_cast<std::uint8_t>(i)), "Scene " + std::to_string(i));
        }
        ADS::Data::SceneData* s1 = project->getSceneData()[0].get();
        s1->setImage("crypt_entrance.png");
        s1->setStartScene(true);
        s1->setState(StateId(1));
        ADS::Data::SceneData::Exits exits;
        exits.north = SceneId(2);
        exits.east = SceneId(3);
        s1->setExits(exits);
        s1->setPresentItemIds({ObjectId(1), ObjectId(2)});
        s1->setDescriptions(ADS::Data::Descriptions{.normal = 10, .longText = 11, .odor = 12});
        s1->addTrigger(1, ADS::Types::EventId(7));

        project->addCharacter(CharacterId(1), "Hero");
        project->addCharacter(CharacterId(2), "Guardian");
        ADS::Data::CharacterData* hero = project->getCharacterData()[0].get();
        hero->setPlayer(true);
        hero->setInitialSceneId(SceneId(1));
        hero->setCapacities(ADS::Data::Capacities{.load = 30, .life = 100, .stamina = 80, .sanity = 90});
        hero->setDialogColor(ADS::Types::Color{0.2f, 0.4f, 0.8f, 1.0f});
        hero->setImagePath("hero.png");
        hero->addTrigger(0x08, ADS::Types::EventId(3));

        project->addItem(ObjectId(1), "Ancient Key");
        project->addItem(ObjectId(2), "Torch");
        ADS::Data::ItemData* key = project->getItemData()[0].get();
        key->setWeight(3);
        key->setSlots(1);
        key->setAffordances({
            {"Takeable", {"on_pickup", "on_drop"}, "ITEM.AFF_TAKEABLE"},
            {"Openable", {"on_open", "on_close"}}});
        key->setSynonyms({"key", "old key"});
        key->setInitialSceneId(SceneId(1));
        key->setDamageEffect(ADS::Data::Effect{.appliedTo = 1, .base = 5});
        key->addTrigger(0x03, ADS::Types::EventId(5));

        project->addState(StateId(1), "Locked");
        project->addState(StateId(2), "Unlocked");
        project->getStateData()[0]->setNext(StateId(2));

        project->addChain(ChainId(1), "Door States");
        project->getChainData()[0]->setHead(StateId(1));

        return project;
    }

} // namespace

TEST_F(ProjectSerializerTest, RichProject_SaveLoadSave_IsByteIdentical)
{
    const auto original = makeRichProject();

    const fs::path first = dir / "crypt.ads";
    const fs::path second = dir / "crypt2.ads";

    ProjectSerializer::save(*original, first);
    const auto loaded = ProjectSerializer::load(first);
    ProjectSerializer::save(*loaded.project, second);

    EXPECT_EQ(readFile(first), readFile(second));
}

TEST_F(ProjectSerializerTest, RichProject_LoadRestoresFieldsAndEntityLinks)
{
    const auto original = makeRichProject();
    const fs::path path = dir / "crypt.ads";
    ProjectSerializer::save(*original, path);

    const auto result = ProjectSerializer::load(path);
    const auto& loaded = result.project;
    EXPECT_TRUE(result.warnings.empty());

    EXPECT_EQ(loaded->getName(), "The Forgotten Crypt");
    EXPECT_EQ(loaded->getScenes().size(), 5u);
    EXPECT_EQ(loaded->getCharacters().size(), 2u);
    EXPECT_EQ(loaded->getItems().size(), 2u);
    EXPECT_EQ(loaded->getStates().size(), 2u);
    EXPECT_EQ(loaded->getChains().size(), 1u);

    EXPECT_EQ(loaded->getGameData(), original->getGameData());

    const ADS::Data::SceneData* s1 = loaded->getSceneData()[0].get();
    EXPECT_EQ(s1->getImage(), "crypt_entrance.png");
    EXPECT_TRUE(s1->isStartScene());
    ASSERT_TRUE(s1->getExits().north.has_value());
    EXPECT_EQ(s1->getExits().north->value, 2);
    ASSERT_TRUE(s1->getState().has_value());
    EXPECT_EQ(s1->getState()->value, 1);
    EXPECT_EQ(s1->getPresentItemIds().size(), 2u);
    ASSERT_TRUE(s1->getTriggers().count(1));
    EXPECT_EQ(s1->getTriggers().at(1).at(0).value, 7);

    const ADS::Data::CharacterData* hero = loaded->getCharacterData()[0].get();
    EXPECT_TRUE(hero->isPlayer());
    ASSERT_TRUE(hero->getInitialSceneId().has_value());
    EXPECT_EQ(hero->getInitialSceneId()->value, 1);
    EXPECT_EQ(hero->getCapacities().life, 100);
    EXPECT_EQ(hero->getDialogColor(), (ADS::Types::Color{0.2f, 0.4f, 0.8f, 1.0f}));
    ASSERT_TRUE(hero->getTriggers().count(0x08));
    EXPECT_EQ(hero->getTriggers().at(0x08).at(0).value, 3);

    const ADS::Data::ItemData* key = loaded->getItemData()[0].get();
    EXPECT_EQ(key->getWeight(), 3);
    ASSERT_EQ(key->getAffordances().size(), 2u);
    EXPECT_EQ(key->getAffordances()[0].name, "Takeable");
    EXPECT_EQ(key->getAffordances()[0].triggers, (std::vector<std::string>{"on_pickup", "on_drop"}));
    EXPECT_EQ(key->getAffordances()[0].presetKey, "ITEM.AFF_TAKEABLE");
    EXPECT_EQ(key->getAffordances()[1].name, "Openable");
    EXPECT_TRUE(key->getAffordances()[1].presetKey.empty());
    EXPECT_EQ(key->getSynonyms().size(), 2u);
    ASSERT_TRUE(key->getTriggers().count(0x03));
    EXPECT_EQ(key->getTriggers().at(0x03).at(0).value, 5);

    ASSERT_TRUE(loaded->getStateData()[0]->getNext().has_value());
    EXPECT_EQ(loaded->getStateData()[0]->getNext()->value, 2);
    ASSERT_TRUE(loaded->getChainData()[0]->getHead().has_value());
    EXPECT_EQ(loaded->getChainData()[0]->getHead()->value, 1);

    // Entity adapters must point back at the loaded project.
    EXPECT_EQ(loaded->getScenes()[0]->getProject(), loaded.get());
}

TEST_F(ProjectSerializerTest, EmptyProject_RoundTrips)
{
    Project empty("Blank");
    const fs::path path = dir / "blank.ads";

    ProjectSerializer::save(empty, path);
    const auto loaded = ProjectSerializer::load(path);

    EXPECT_TRUE(loaded.warnings.empty());
    EXPECT_EQ(loaded.project->getName(), "Blank");
    EXPECT_TRUE(loaded.project->getScenes().empty());
    EXPECT_TRUE(loaded.project->getItems().empty());
}

TEST_F(ProjectSerializerTest, Load_MissingFile_ThrowsFileNotFound)
{
    EXPECT_THROW(ProjectSerializer::load(dir / "does_not_exist.ads"),
                 ADS::Exceptions::file_not_found_exception);
}

TEST_F(ProjectSerializerTest, Load_MalformedJson_Throws)
{
    const fs::path path = dir / "bad.ads";
    std::ofstream(path) << "{ this is not json ";

    EXPECT_THROW(ProjectSerializer::load(path),
                 ADS::Exceptions::project_serialization_exception);
}

TEST_F(ProjectSerializerTest, Load_MissingRequiredKey_Throws)
{
    const fs::path path = dir / "partial.ads";
    std::ofstream(path) << R"({"format":"ads-project","schemaVersion":1,"name":"X"})";

    EXPECT_THROW(ProjectSerializer::load(path),
                 ADS::Exceptions::project_serialization_exception);
}

TEST_F(ProjectSerializerTest, Load_UnsupportedSchemaVersion_Throws)
{
    const fs::path good = dir / "good.ads";
    ProjectSerializer::save(*makeRichProject(), good);

    std::string text;
    {
        std::ifstream in(good);
        text.assign(std::istreambuf_iterator<char>(in), {});
    }
    const auto pos = text.find("\"schemaVersion\": 1");
    ASSERT_NE(pos, std::string::npos);
    text.replace(pos, std::string("\"schemaVersion\": 1").size(), "\"schemaVersion\": 99");

    const fs::path bumped = dir / "v99.ads";
    std::ofstream(bumped) << text;

    EXPECT_THROW(ProjectSerializer::load(bumped),
                 ADS::Exceptions::project_serialization_exception);
}

TEST_F(ProjectSerializerTest, SavedFile_CarriesAChecksum)
{
    const fs::path path = dir / "sum.ads";
    ProjectSerializer::save(*makeRichProject(), path);

    const std::string text = readFile(path);
    EXPECT_NE(text.find("\"checksum\""), std::string::npos);
}

TEST_F(ProjectSerializerTest, Load_TamperedContent_ThrowsChecksumMismatch)
{
    const fs::path path = dir / "tampered.ads";
    ProjectSerializer::save(*makeRichProject(), path);

    std::string text = readFile(path);
    // Change a value that is covered by the checksum but leaves valid JSON and a
    // valid schema version.
    const auto pos = text.find("A lone adventurer");
    ASSERT_NE(pos, std::string::npos);
    text.replace(pos, 1, "a");
    { std::ofstream out(path, std::ios::trunc | std::ios::binary); out << text; }

    EXPECT_THROW(ProjectSerializer::load(path),
                 ADS::Exceptions::project_serialization_exception);
}

TEST_F(ProjectSerializerTest, Load_NoChecksumKey_LoadsWithWarning)
{
    const fs::path path = dir / "nosum.ads";
    ProjectSerializer::save(*makeRichProject(), path);

    std::string text = readFile(path);
    // Drop the whole checksum line. It sorts alphabetically between "characters"
    // and "format" (never last), so its line ends with a comma and removing it
    // leaves valid JSON.
    const auto keyPos = text.find("\"checksum\":");
    ASSERT_NE(keyPos, std::string::npos);
    const auto lineStart = text.rfind('\n', keyPos) + 1;
    const auto lineEnd = text.find('\n', keyPos);
    text.erase(lineStart, lineEnd - lineStart + 1);
    { std::ofstream out(path, std::ios::trunc | std::ios::binary); out << text; }

    LoadResult loaded;
    EXPECT_NO_THROW(loaded = ProjectSerializer::load(path));
    ASSERT_NE(loaded.project, nullptr);
    EXPECT_TRUE(loaded.warnings.empty());
    EXPECT_EQ(loaded.project->getName(), "The Forgotten Crypt");
}

/// Remove the whole "checksum" line from a saved `.ads`'s text, so subsequent
/// hand-edits don't trip the (intentionally hard-abort) checksum-mismatch path
/// and per-entity parsing gets a chance to run instead — mirrors how a file
/// with its checksum stripped is already treated as "unverified, not fatal".
std::string stripChecksumLine(std::string text)
{
    const auto keyPos = text.find("\"checksum\":");
    if (keyPos == std::string::npos) {
        return text;
    }
    const auto lineStart = text.rfind('\n', keyPos) + 1;
    const auto lineEnd = text.find('\n', keyPos);
    text.erase(lineStart, lineEnd - lineStart + 1);
    return text;
}

TEST_F(ProjectSerializerTest, Load_OneEntityMissingRequiredField_SkipsItAndLoadsRest)
{
    const fs::path path = dir / "onebad.ads";
    ProjectSerializer::save(*makeRichProject(), path);

    std::string text = stripChecksumLine(readFile(path));
    // Scene 2 loses its "name" field — Data::applyJson requires it — while
    // scenes 1/3/4/5 and every other collection stay untouched.
    const auto pos = text.find(R"("name": "Scene 2")");
    ASSERT_NE(pos, std::string::npos);
    text.replace(pos, std::string(R"("name": "Scene 2")").size(), R"("name_typo": "Scene 2")");
    { std::ofstream out(path, std::ios::trunc | std::ios::binary); out << text; }

    LoadResult result;
    EXPECT_NO_THROW(result = ProjectSerializer::load(path));
    ASSERT_NE(result.project, nullptr);

    // 4 of 5 scenes survive; the bad one is reported, not silently dropped.
    EXPECT_EQ(result.project->getScenes().size(), 4u);
    EXPECT_EQ(result.project->getCharacters().size(), 2u);
    ASSERT_EQ(result.warnings.size(), 1u);
    EXPECT_EQ(result.warnings[0].entityKind, "Scene");
}

TEST_F(ProjectSerializerTest, Load_DuplicateEntityId_SkipsSecondAndLoadsRest)
{
    const fs::path path = dir / "dup.ads";
    ProjectSerializer::save(*makeRichProject(), path);

    std::string text = stripChecksumLine(readFile(path));
    // Character 2 ("Guardian") reuses id 1, already taken by "Hero". Locate
    // the nearest preceding "id" field to "Guardian"'s own name field, since
    // scenes also contain "id": 2 substrings earlier in the file.
    const auto guardianPos = text.find(R"("name": "Guardian")");
    ASSERT_NE(guardianPos, std::string::npos);
    const auto idPos = text.rfind(R"("id": 2)", guardianPos);
    ASSERT_NE(idPos, std::string::npos);
    text.replace(idPos, std::string(R"("id": 2)").size(), R"("id": 1)");
    { std::ofstream out(path, std::ios::trunc | std::ios::binary); out << text; }

    LoadResult result;
    EXPECT_NO_THROW(result = ProjectSerializer::load(path));
    ASSERT_NE(result.project, nullptr);

    EXPECT_EQ(result.project->getCharacters().size(), 1u);
    EXPECT_EQ(result.project->getScenes().size(), 5u);
    ASSERT_EQ(result.warnings.size(), 1u);
    EXPECT_EQ(result.warnings[0].entityKind, "Character");
}

TEST_F(ProjectSerializerTest, Save_WritesSiblingTrn_WithoutDescriptionTextsInAds)
{
    auto project = makeRichProject();
    project->getScenes()[0]->setName("La entrada"); // default-lang name
    ADS::Core::TranslationCatalog::set(*project, "scene.1.desc.normal", "es_ES",
                                       "Un umbral de piedra.");

    const fs::path adsPath = dir / "crypt.ads";
    ProjectSerializer::save(*project, adsPath);

    const fs::path trnPath = dir / "crypt.trn";
    ASSERT_TRUE(fs::exists(trnPath));

    // .ads no longer carries per-language draft text.
    EXPECT_EQ(readFile(adsPath).find("descriptionTexts"), std::string::npos);

    // .trn carries it.
    const std::string trn = readFile(trnPath);
    EXPECT_NE(trn.find("\"ads-translations\""), std::string::npos);
    EXPECT_NE(trn.find("Un umbral de piedra."), std::string::npos);
}

TEST_F(ProjectSerializerTest, SaveThenLoad_RestoresTranslationsFromTrn)
{
    auto project = makeRichProject();
    ADS::Core::TranslationCatalog::set(*project, "scene.1.name", "es_ES", "La entrada");
    ADS::Core::TranslationCatalog::set(*project, "item.1.desc.odor", "en_US", "Rust.");

    const fs::path adsPath = dir / "crypt.ads";
    ProjectSerializer::save(*project, adsPath);

    const auto loaded = ProjectSerializer::load(adsPath);
    ASSERT_NE(loaded.project, nullptr);
    EXPECT_EQ(ADS::Core::TranslationCatalog::get(*loaded.project, "scene.1.name", "es_ES"), "La entrada");
    EXPECT_EQ(ADS::Core::TranslationCatalog::get(*loaded.project, "item.1.desc.odor", "en_US"), "Rust.");
}
