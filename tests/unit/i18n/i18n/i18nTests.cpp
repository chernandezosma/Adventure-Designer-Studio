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

#include "i18nTests.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include "i18n/i18n.h"

using namespace ADS::i18n;
using namespace std;

namespace fs = std::filesystem;

namespace {
    constexpr const char* kFallback = "es_ES";
    constexpr const char* kOtherLanguage = "en_US";
    constexpr const char* kUnsupportedLanguage = "xx_XX";

    void setEnvVar(const std::string& name, const std::string& value)
    {
#ifdef _WIN32
        _putenv_s(name.c_str(), value.c_str());
#else
        setenv(name.c_str(), value.c_str(), 1);
#endif
    }
}

// =============================================================================
// i18nTests fixture
// =============================================================================

void i18nTests::SetUp()
{
    testDir = fs::temp_directory_path() / fs::path("ads_i18n_tests_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
    fs::remove_all(testDir);
    fs::create_directories(testDir);
    baseFolder = testDir.string();

    const char* envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    for (const char* var : envVars) {
        if (const char* value = std::getenv(var)) {
            originalEnv[var] = value;
        }
        unsetenv(var);
    }
    // Force a locale i18n cannot map to a supported language, so construction
    // deterministically falls back to the fallback language passed to i18n().
    setEnvVar("LC_ALL", "C");
}

void i18nTests::TearDown()
{
    fs::remove_all(testDir);

    const char* envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    for (const char* var : envVars) {
        unsetenv(var);
    }
    for (const auto& [key, value] : originalEnv) {
        setEnvVar(key, value);
    }
}

void i18nTests::createJsonFile(const std::string& lang, const std::string& jsonContent)
{
    ofstream file(testDir / (lang + ".json"));
    file << jsonContent;
    file.close();
}

// =============================================================================
// Construction
// =============================================================================

TEST_F(i18nTests, Construction_ValidFolderAndFallback_Succeeds)
{
    EXPECT_NO_THROW({
        i18n translator(baseFolder, kFallback);
        EXPECT_EQ(translator.getCurrentLocale().locale, kFallback);
    });
}

TEST_F(i18nTests, Construction_NonExistentFolder_ThrowsTranslationFileException)
{
    std::string missingFolder = (testDir / "does-not-exist").string();

    EXPECT_THROW(i18n(missingFolder, kFallback), translation_file_exception);
}

TEST_F(i18nTests, Construction_UnsupportedFallback_ThrowsLocaleException)
{
    EXPECT_THROW(i18n(baseFolder, kUnsupportedLanguage), locale_exception);
}

// =============================================================================
// setLocale
// =============================================================================

TEST_F(i18nTests, SetLocale_ValidCode_UpdatesCurrentLocaleAndLoadsLanguage)
{
    i18n translator(baseFolder, kFallback);

    translator.setLocale(kOtherLanguage);

    EXPECT_EQ(translator.getCurrentLocale().locale, kOtherLanguage);
    EXPECT_TRUE(translator.hasLanguage(kOtherLanguage));
}

TEST_F(i18nTests, SetLocale_InvalidCode_ThrowsLocaleException)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_THROW(translator.setLocale(kUnsupportedLanguage), locale_exception);
}

TEST_F(i18nTests, SetLocale_InvalidLocaleInfo_ThrowsLocaleException)
{
    i18n translator(baseFolder, kFallback);
    LocaleInfo invalid{kUnsupportedLanguage, "Unsupported"};

    EXPECT_THROW(translator.setLocale(invalid), locale_exception);
}

TEST_F(i18nTests, SetLocale_SupportedCodeWithoutTranslationFile_TranslatesViaFallbackNotRawKey)
{
    // fr_FR is in the language catalog but has no JSON file in this folder —
    // the same shape as the IDE's 7-translated-locales vs 51-catalog-entries
    // situation. Switching to it must degrade to the fallback language's
    // string, and an unknown key must still round-trip to itself (never a
    // partially-translated, key-leaking UI).
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("menu.file.new", "Nuevo", kFallback);

    translator.setLocale("fr_FR");

    EXPECT_EQ(translator.getCurrentLocale().locale, "fr_FR");
    EXPECT_EQ(translator._t("menu.file.new"), "Nuevo");
    EXPECT_EQ(translator._t("totally.absent.key"), "totally.absent.key");
}

// =============================================================================
// hasLanguage / addLanguage
// =============================================================================

TEST_F(i18nTests, HasLanguage_NotLoaded_ReturnsFalse)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_FALSE(translator.hasLanguage(kOtherLanguage));
}

TEST_F(i18nTests, AddLanguage_ValidUnloadedWithFile_LoadsTranslationsFromDisk)
{
    createJsonFile(kOtherLanguage, R"({"hello": "Hello", "goodbye": "Goodbye"})");
    i18n translator(baseFolder, kFallback);

    translator.addLanguage(kOtherLanguage);

    EXPECT_TRUE(translator.hasLanguage(kOtherLanguage));
    auto translations = translator.getTranslations(kOtherLanguage);
    EXPECT_EQ(translations.at("hello"), "Hello");
    EXPECT_EQ(translations.at("goodbye"), "Goodbye");
}

TEST_F(i18nTests, AddLanguage_ValidUnloadedNoFile_CreatesEmptyTranslationSet)
{
    i18n translator(baseFolder, kFallback);

    translator.addLanguage(kOtherLanguage);

    EXPECT_TRUE(translator.hasLanguage(kOtherLanguage));
    EXPECT_TRUE(translator.getTranslations(kOtherLanguage).empty());
}

TEST_F(i18nTests, AddLanguage_Unsupported_ThrowsLocaleException)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_THROW(translator.addLanguage(kUnsupportedLanguage), locale_exception);
}

TEST_F(i18nTests, AddLanguage_AlreadyLoaded_DoesNotThrowAndKeepsData)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hello", kOtherLanguage);

    EXPECT_NO_THROW(translator.addLanguage(kOtherLanguage));
    EXPECT_EQ(translator.getTranslations(kOtherLanguage).at("hello"), "Hello");
}

TEST_F(i18nTests, AddLanguage_MalformedJsonFile_LoadsEmptyWithoutThrowing)
{
    createJsonFile(kOtherLanguage, "{not valid json");
    i18n translator(baseFolder, kFallback);

    EXPECT_NO_THROW(translator.addLanguage(kOtherLanguage));
    EXPECT_TRUE(translator.hasLanguage(kOtherLanguage));
    EXPECT_TRUE(translator.getTranslations(kOtherLanguage).empty());
}

TEST_F(i18nTests, AddLanguage_NestedJson_FlattensKeysWithDotNotation)
{
    createJsonFile(kOtherLanguage, R"({"menu": {"file": "File", "edit": "Edit"}})");
    i18n translator(baseFolder, kFallback);

    translator.addLanguage(kOtherLanguage);

    auto translations = translator.getTranslations(kOtherLanguage);
    EXPECT_EQ(translations.at("menu.file"), "File");
    EXPECT_EQ(translations.at("menu.edit"), "Edit");
}

// =============================================================================
// getAvailableLanguages / getSupportedLanguages
// =============================================================================

TEST_F(i18nTests, GetAvailableLanguages_ReturnsSortedLoadedLanguages)
{
    i18n translator(baseFolder, kFallback);
    translator.addLanguage(kOtherLanguage);

    auto available = translator.getAvailableLanguages();

    EXPECT_TRUE(std::is_sorted(available.begin(), available.end()));
    EXPECT_NE(std::find(available.begin(), available.end(), kFallback), available.end());
    EXPECT_NE(std::find(available.begin(), available.end(), kOtherLanguage), available.end());
}

TEST_F(i18nTests, GetSupportedLanguages_ContainsKnownCodes)
{
    auto supported = i18n::getSupportedLanguages();

    EXPECT_NE(std::find(supported.begin(), supported.end(), kFallback), supported.end());
    EXPECT_NE(std::find(supported.begin(), supported.end(), kOtherLanguage), supported.end());
}

// =============================================================================
// addTranslation / translate / _t
// =============================================================================

TEST_F(i18nTests, AddTranslation_NewKey_TShortcutReturnsValue)
{
    i18n translator(baseFolder, kFallback);

    translator.addTranslation("hello", "Hola", kFallback);

    EXPECT_EQ(translator._t("hello"), "Hola");
}

TEST_F(i18nTests, T_Shortcut_MissingInTargetLanguage_FallsBackToFallbackLanguage)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);
    translator.setLocale(kOtherLanguage);

    EXPECT_EQ(translator._t("hello"), "Hola");
}

TEST_F(i18nTests, T_Shortcut_MissingEverywhere_ReturnsKeyItself)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_EQ(translator._t("no.such.key"), "no.such.key");
}

// =============================================================================
// translatePlural / translateWithParams
// =============================================================================

TEST_F(i18nTests, TranslatePlural_CountOne_UsesSingularKey)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("items_singular", "1 item", kFallback);
    translator.addTranslation("items_plural", "{count} items", kFallback);

    EXPECT_EQ(translator.translatePlural("items_singular", "items_plural", 1, kFallback), "1 item");
}

TEST_F(i18nTests, TranslatePlural_CountOther_UsesPluralKey)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("items_singular", "1 item", kFallback);
    translator.addTranslation("items_plural", "{count} items", kFallback);

    EXPECT_EQ(translator.translatePlural("items_singular", "items_plural", 5, kFallback), "{count} items");
}

TEST_F(i18nTests, TranslateWithParams_SubstitutesPlaceholder)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("welcome", "Welcome {name}", kFallback);

    std::string result = translator.translateWithParams("welcome", {{"name", "Ada"}}, kFallback);

    EXPECT_EQ(result, "Welcome Ada");
}

TEST_F(i18nTests, TranslateWithParams_UnmatchedPlaceholderLeftUnchanged)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("welcome", "Welcome {name}", kFallback);

    std::string result = translator.translateWithParams("welcome", {{"other", "value"}}, kFallback);

    EXPECT_EQ(result, "Welcome {name}");
}

// =============================================================================
// reloadTranslations
// =============================================================================

TEST_F(i18nTests, ReloadTranslations_ReturnsCountOfLanguagesLoadedFromDisk)
{
    createJsonFile(kFallback, R"({"hello": "Hola"})");
    i18n translator(baseFolder, kFallback);

    size_t reloaded = translator.reloadTranslations();

    EXPECT_EQ(reloaded, 1u);
    EXPECT_EQ(translator.getTranslations(kFallback).at("hello"), "Hola");
}

// =============================================================================
// saveTranslations
// =============================================================================

TEST_F(i18nTests, SaveTranslations_UnloadedLanguage_ReturnsFalse)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_FALSE(translator.saveTranslations(kOtherLanguage));
}

TEST_F(i18nTests, SaveTranslations_UseExistingFalse_ReturnsFalse)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);

    EXPECT_FALSE(translator.saveTranslations(kFallback, false));
}

TEST_F(i18nTests, SaveTranslations_UseExistingTrue_WritesFileAndReturnsTrue)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);

    bool saved = translator.saveTranslations(kFallback, true);

    ASSERT_TRUE(saved);
    ASSERT_TRUE(fs::exists(testDir / (std::string(kFallback) + ".json")));

    translator.reloadTranslations();
    EXPECT_EQ(translator.getTranslations(kFallback).at("hello"), "Hola");
}

TEST_F(i18nTests, SaveTranslations_DottedKey_WritesNestedJson)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("menu.file", "File", kFallback);

    bool saved = translator.saveTranslations(kFallback, true);

    ASSERT_TRUE(saved);
    translator.reloadTranslations();
    EXPECT_EQ(translator.getTranslations(kFallback).at("menu.file"), "File");
}

// =============================================================================
// getTranslations / getFallbackLanguageTranslations / getLanguage
// =============================================================================

TEST_F(i18nTests, GetTranslations_LanguageNotLoaded_ReturnsEmptyMap)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_TRUE(translator.getTranslations(kUnsupportedLanguage).empty());
}

TEST_F(i18nTests, GetFallbackLanguageTranslations_ReturnsFallbackEntry)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);

    auto* fallbackEntry = translator.getFallbackLanguageTranslations();

    ASSERT_NE(fallbackEntry, nullptr);
    EXPECT_EQ(fallbackEntry->first, kFallback);
    EXPECT_EQ(fallbackEntry->second.at("hello"), "Hola");
}

TEST_F(i18nTests, GetLanguage_Loaded_ReturnsEntry)
{
    i18n translator(baseFolder, kFallback);

    auto* entry = translator.getLanguage(kFallback);

    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->first, kFallback);
}

TEST_F(i18nTests, GetLanguage_NotLoaded_ReturnsNullptr)
{
    i18n translator(baseFolder, kFallback);

    EXPECT_EQ(translator.getLanguage(kOtherLanguage), nullptr);
}

// =============================================================================
// addTranslation with fallback translation
// =============================================================================

TEST_F(i18nTests, AddTranslation_WithFallbackTranslation_AddsToBothLanguages)
{
    i18n translator(baseFolder, kFallback);

    translator.addTranslation("hello", "Hello", kOtherLanguage, "Hola");

    EXPECT_EQ(translator.getTranslations(kOtherLanguage).at("hello"), "Hello");
    EXPECT_EQ(translator.getTranslations(kFallback).at("hello"), "Hola");
}

// =============================================================================
// reloadTranslations with a language that has no file on disk
// =============================================================================

TEST_F(i18nTests, ReloadTranslations_LanguageWithoutFile_KeepsEmptyMap)
{
    i18n translator(baseFolder, kFallback);
    translator.addLanguage(kOtherLanguage);

    size_t reloaded = translator.reloadTranslations();

    EXPECT_LT(reloaded, translator.getAvailableLanguages().size());
    EXPECT_TRUE(translator.hasLanguage(kOtherLanguage));
    EXPECT_TRUE(translator.getTranslations(kOtherLanguage).empty());
}

// =============================================================================
// getTranslationStats / findMissingTranslations
// =============================================================================

TEST_F(i18nTests, GetTranslationStats_ReturnsCountPerLoadedLanguage)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);
    translator.addTranslation("goodbye", "Adios", kFallback);
    translator.addLanguage(kOtherLanguage);

    auto stats = translator.getTranslationStats();

    EXPECT_EQ(stats.at(kFallback), 2u);
    EXPECT_EQ(stats.at(kOtherLanguage), 0u);
}

TEST_F(i18nTests, FindMissingTranslations_ReturnsKeysPresentInFallbackOnly)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);
    translator.addTranslation("goodbye", "Adios", kFallback);
    translator.addLanguage(kOtherLanguage);
    translator.addTranslation("hello", "Hello", kOtherLanguage);

    auto missing = translator.findMissingTranslations(kOtherLanguage);

    ASSERT_EQ(missing.size(), 1u);
    EXPECT_EQ(missing[0], "goodbye");
}

TEST_F(i18nTests, FindMissingTranslations_TargetLanguageNotLoaded_ReturnsEmpty)
{
    i18n translator(baseFolder, kFallback);
    translator.addTranslation("hello", "Hola", kFallback);

    EXPECT_TRUE(translator.findMissingTranslations(kOtherLanguage).empty());
}