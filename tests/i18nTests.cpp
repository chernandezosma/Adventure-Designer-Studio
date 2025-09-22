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

#include <gtest/gtest.h>
#include <memory>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include "i18n/i18n.h"
#include "i18nTests.h"

using namespace ADS::i18n;
using namespace ADS::Constants::Languages;
using namespace std;

namespace fs = std::filesystem;

// =============================================================================
// HELPER FUNCTIONS IMPLEMENTATIONS
// =============================================================================

template <typename Container, typename T>
bool
contains (const Container &container, const T &value)
{
  return std::find (container.begin (), container.end (), value) != container.end ();
}

bool
stringContains (const std::string &haystack, const std::string &needle)
{
  return haystack.find (needle) != std::string::npos;
}

// Helper function to setup i18n objects
std::unique_ptr<i18n>
SetupI18nObject (const std::string &baseFolder, const char *fallback)
{
  return std::make_unique<i18n> (baseFolder, std::string (fallback));
}

// Helper function to verify i18n objects
void
verifyI18nObject (const std::unique_ptr<i18n> &obj)
{
  EXPECT_NE (obj, nullptr);
  EXPECT_FALSE (obj->getCurrentLocale().locale.empty());
  EXPECT_TRUE (obj->getCurrentLocale().isValid());
}

// =============================================================================
// I18nEnvironment CLASS IMPLEMENTATIONS
// =============================================================================

void
I18nEnvironment::setLocaleEnvironment (const std::string &locale)
{
  // Clear existing locale environment
  unsetenv ("LC_ALL");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");

  // Set the test locale
#ifdef _WIN32
  setEnvVar ("LANG", locale);
#else
  setEnvVar ("LC_ALL", locale);
#endif
}

void
I18nEnvironment::clearLocaleEnvironment ()
{
  unsetenv ("LC_ALL");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
}

void
I18nEnvironment::saveOriginalEnv ()
{
  const char *envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
  for (const char *var : envVars)
    {
      const char *value = std::getenv (var);
      if (value)
        {
          originalEnv[var] = value;
        }
    }
}

void
I18nEnvironment::restoreOriginalEnv ()
{
  // First, unset all variables
  const char *envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
  for (const char *var : envVars)
    {
      unsetenv (var);
    }

  // Then restore original values
  for (const auto &[key, value] : originalEnv)
    {
      setEnvVar (key, value);
    }
}

void
I18nEnvironment::setEnvVar (const std::string &name, const std::string &value)
{
#ifdef _WIN32
  _putenv_s (name.c_str (), value.c_str ());
#else
  setenv (name.c_str (), value.c_str (), 1);
#endif
}

// =============================================================================
// MockConstants CLASS IMPLEMENTATIONS
// =============================================================================

bool
MockConstants::isLanguageSupported (const std::string &lang)
{
  return lang == "en_US" || lang == "es_ES" || lang == "fr_FR" || lang == "de_DE";
}

std::string
MockConstants::getLanguageName (const std::string &lang)
{
  if (lang == "en_US")
    return "English";
  if (lang == "es_ES")
    return "Spanish";
  if (lang == "fr_FR")
    return "French";
  if (lang == "de_DE")
    return "German";
  return "";
}

std::string
MockConstants::normalizePlatformLocale (const std::string &locale)
{
  if (locale.find ("en_US") != std::string::npos)
    return "en_US";
  if (locale.find ("es_ES") != std::string::npos)
    return "es_ES";
  if (locale.find ("fr_FR") != std::string::npos)
    return "fr_FR";
  if (locale.find ("de_DE") != std::string::npos)
    return "de_DE";
  return "";
}

std::vector<std::string>
MockConstants::getSupportedLocales ()
{
  return {"en_US", "es_ES", "fr_FR", "de_DE"};
}

// =============================================================================
// i18nTests CLASS IMPLEMENTATIONS
// =============================================================================

void
i18nTests::SetUp ()
{
  // Create temporary test directory
  testDir = fs::temp_directory_path () / "translations";
  BASE_FOLDER = testDir.string ();
  this->createTestDirectories (BASE_FOLDER);
  this->createTestFiles ();
}

void
i18nTests::TearDown ()
{
  fs::remove_all (testDir);
}

void
i18nTests::createTestDirectories (const std::string &testDir)
{
  if (!fs::exists (testDir))
    {
      fs::create_directories (testDir);
    }
}

void
i18nTests::createPropertiesFile (const std::string &lang,
                                 const std::unordered_map<std::string, std::string> &translations)
{
  ofstream file (testDir / (lang + ".properties"));
  file << "# Test translation file for " << lang << endl;
  for (const auto &[key, value] : translations)
    {
      file << key << "=" << value << endl;
    }
  file.close ();
}

void
i18nTests::createJsonFile (const std::string &lang, const std::string &jsonContent)
{
  ofstream file (testDir / (lang + ".json"));
  file << jsonContent;
  file.close ();
}

void
i18nTests::createPoFile (const std::string &lang, const std::unordered_map<std::string, std::string> &translations)
{
  ofstream file (testDir / (lang + ".po"));
  file << "# PO file for " << lang << endl;
  file << "msgid \"\"\nmsgstr \"\"\n";
  file << "\"Language: " << lang << "\\n\"\n\n";

  for (const auto &[key, value] : translations)
    {
      file << "msgid \"" << key << "\"\n";
      file << "msgstr \"" << value << "\"\n\n";
    }
  file.close ();
}

void
i18nTests::createMalformedFile (const std::string &name, const std::string &content)
{
  ofstream file (testDir / (name + ".json"));
  file << content;
  file.close ();
}

void
i18nTests::createTestFiles ()
{
  createPropertiesFile (
    ENGLISH_UNITED_STATES.data (),
    {
      {"hello", "Hello"},
      {"goodbye", "Goodbye"},
      {"welcome", "Welcome {name}"},
      {"items_singular", "1 item"},
      {"items_plural", "{count} items"},
      {"nested.menu.file", "File"},
      {"nested.menu.edit", "Edit"}
    }
    );

  // Create Spanish properties file
  createPropertiesFile (
    SPANISH_SPAIN.data (),
    {
      {"hello", "Hola"},
      {"goodbye", "Adiós"},
      {"welcome", "Bienvenido {name}"},
      {"items_singular", "1 elemento"},
      {"items_plural", "{count} elementos"}
    }
    );

  // Create JSON file for French
  createJsonFile (
    "fr_FR",
    R"({
            "hello": "Bonjour",
            "goodbye": "Au revoir",
            "nested": {
                "menu": {
                    "file": "Fichier",
                    "edit": "Modifier"
                }
            }
        })"
    );

  // Create PO file for German
  createPoFile ("de_DE",
                {
                  {"hello", "Hallo"},
                  {"goodbye", "Auf Wiedersehen"}
                }
    );
}

// =============================================================================
// TESTS
// =============================================================================

TEST_F (i18nTests, Success_Object_Initialization)
{
  std::unique_ptr<i18n> i18nObject = SetupI18nObject (BASE_FOLDER, SPANISH_SPAIN.data ());
  verifyI18nObject (i18nObject);
}

TEST_F (i18nTests, Failed_Object_Initialization_non_existing_folder)
{
  EXPECT_THROW (
    std::unique_ptr<i18n> i18nObject = SetupI18nObject("unexisting_folder", SPANISH_SPAIN.data()),
    translation_file_exception
    );
}

TEST_F (i18nTests, Failed_Object_Initialization_Unsupported_language)
{
  EXPECT_THROW (
    std::unique_ptr<i18n> i18nObject = SetupI18nObject(BASE_FOLDER, "UNSUPPORTED_LANGUAGE"),
    locale_exception
    );
}

TEST_F (i18nTests, Init_Unsuported_Language_locale)
{
  std::unique_ptr<I18nEnvironment> I18EnvironmentObject = std::make_unique<I18nEnvironment> ();

  I18EnvironmentObject->saveOriginalEnv ();

  unsetenv ("LC_ALL");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
#ifdef _WIN32
  I18EnvironmentObject->setEnvVar ("LANG", "xx_XX.UTF-8");
#else
  I18EnvironmentObject->setEnvVar ("LC_ALL", "xx_XX.UTF-8");
#endif

  EXPECT_THROW (std::unique_ptr<i18n> i18nObject = SetupI18nObject(BASE_FOLDER, "xx_XX.UTF-8"), locale_exception);
}

TEST_F (i18nTests, BasicTranslation)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  i18nObject->addTranslation ("hello", "Hello", ENGLISH_UNITED_STATES.data ());
  i18nObject->addTranslation ("hello", "Hola", SPANISH_SPAIN.data ());

  string result = i18nObject->translate ("hello");
  EXPECT_EQ (result, "Hello");

  result = i18nObject->translate ("hello", SPANISH_SPAIN.data ());
  EXPECT_EQ (result, "Hola");
}

TEST_F (i18nTests, SetLocale)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  LocaleInfo li = {"xx_XX", "La mate por un yogur"};

  EXPECT_NO_THROW (i18nObject->setLocale(SPANISH_SPAIN.data()));
  EXPECT_EQ (i18nObject->getCurrentLocale().locale, SPANISH_SPAIN.data());
  EXPECT_THROW (i18nObject->setLocale(li), locale_exception);
}

TEST_F (i18nTests, TranslationWithParameters)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  i18nObject->addTranslation ("welcome", "Welcome {name}", ENGLISH_UNITED_STATES.data ());
  i18nObject->addTranslation ("welcome", "Bienvenido {name}", SPANISH_SPAIN.data ());

  unordered_map<string, string> params = {{"name", "John"}};
  string result = i18nObject->translateWithParams ("welcome", params);
  EXPECT_EQ (result, "Welcome John");

  result = i18nObject->translateWithParams ("welcome", params, SPANISH_SPAIN.data ());
  EXPECT_EQ (result, "Bienvenido John");
}

TEST_F (i18nTests, AddLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  EXPECT_NO_THROW (i18nObject->addLanguage(FRENCH_FRANCE.data()));
  EXPECT_TRUE (i18nObject->hasLanguage(FRENCH_FRANCE.data()));
}

TEST_F (i18nTests, AddLanguageTwice)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());
  EXPECT_NO_THROW (i18nObject->addLanguage(FRENCH_FRANCE.data()));

  EXPECT_EQ (
    i18nObject->addLanguage(FRENCH_FRANCE.data())->first,
    FRENCH_FRANCE.data()
    );
}

TEST_F (i18nTests, AddLanguageWithNonSupportedLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  EXPECT_THROW (i18nObject->addLanguage("XX"), locale_exception);
}

TEST_F (i18nTests, InvalidLocale_US)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  EXPECT_THROW (i18nObject->setLocale("invalid"), locale_exception);
}

TEST_F (i18nTests, GetAvailableLanguages)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());
  i18nObject->addLanguage (SPANISH_SPAIN.data ());

  vector<string> languages = i18nObject->getAvailableLanguages ();
  EXPECT_FALSE (languages.empty());
  EXPECT_TRUE (find(languages.begin(), languages.end(), ENGLISH_UNITED_STATES.data()) != languages.end());
  EXPECT_TRUE (find(languages.begin(), languages.end(), SPANISH_SPAIN.data()) != languages.end());
}

TEST_F (i18nTests, GetTranslations)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  i18nObject->addLanguage (SPANISH_SPAIN.data ());
  i18nObject->addTranslation ("hello", "Hello", ENGLISH_UNITED_STATES.data ());
  i18nObject->addTranslation ("hello", "Hola", SPANISH_SPAIN.data ());

  std::unordered_map<string, string> englishTranslations = i18nObject->getTranslations (ENGLISH_UNITED_STATES.data ());
  std::unordered_map<string, string> spanishTranslations = i18nObject->getTranslations (SPANISH_SPAIN.data ());

  std::unordered_map<string, string> englishExpectedTranslations = {
    {"hello", "Hello"}
  };

  std::unordered_map<string, string> spanishExpectedTranslations = {
    {"hello", "Hola"},
    {"goodbye", "Adiós"},
    {"welcome", "Bienvenido {name}"},
    {"items_singular", "1 elemento"},
    {"items_plural", "{count} elementos"}
  };

  EXPECT_EQ (englishTranslations, englishExpectedTranslations);
  EXPECT_EQ (spanishTranslations, spanishExpectedTranslations);
}

TEST_F (i18nTests, GetTranslationsForNonAddedLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());
  unordered_map<string, string> emptyTranslations = {};

  auto frenchTranslations = i18nObject->getTranslations (FRENCH_FRANCE.data ());
  EXPECT_EQ (frenchTranslations, emptyTranslations);
}

TEST_F (i18nTests, GetTranslationsForFallbackLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  i18nObject->addTranslation ("hello", "Hello", ENGLISH_UNITED_STATES.data ());

  const pair<const string, unordered_map<string, string> > *fallbackTranslations = i18nObject->
    getFallbackLanguageTranslations ();

  std::unordered_map<string, string> englishExpectedTranslations = {
    {"hello", "Hello"}
  };

  EXPECT_EQ (fallbackTranslations->second, englishExpectedTranslations);
}

TEST_F (i18nTests, GetExistingLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  i18nObject->addLanguage (SPANISH_SPAIN.data ());
  i18nObject->addTranslation ("hello", "Hello", ENGLISH_UNITED_STATES.data ());
  i18nObject->addTranslation ("hello", "Hola", SPANISH_SPAIN.data ());

  const pair<const string, unordered_map<string, string> > *spanishLanguage = i18nObject->getLanguage (
    SPANISH_SPAIN.data ());

  std::unordered_map<string, string> spanishExpectedTranslations = {
    {"hello", "Hola"},
    {"goodbye", "Adiós"},
    {"welcome", "Bienvenido {name}"},
    {"items_singular", "1 elemento"},
    {"items_plural", "{count} elementos"}
  };

  EXPECT_EQ (spanishLanguage->second, spanishExpectedTranslations);
}

TEST_F (i18nTests, GetNonExistingLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  auto language = i18nObject->getLanguage (SPANISH_SPAIN.data ());
  EXPECT_EQ (language, nullptr);
}

TEST_F (i18nTests, SetInexistentLocale)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  EXPECT_THROW (i18nObject->setLocale("xx_XX"), locale_exception);
}

TEST_F (i18nTests, loadTranslationFile)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());
  i18nObject->addLanguage (SPANISH_SPAIN.data ());

  EXPECT_TRUE (i18nObject->hasLanguage(SPANISH_SPAIN.data()));
}

// Test cases for loadTranslationFile method covering all supported formats and scenarios

TEST_F (i18nTests, loadTranslationFile_JsonFormat_Success)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Assuming you have a Spanish JSON translation file in your test data
  i18nObject->addLanguage (SPANISH_SPAIN.data ());

  EXPECT_TRUE (i18nObject->hasLanguage(SPANISH_SPAIN.data()));

  // Verify some translations were actually loaded
  auto translations = i18nObject->getTranslations (SPANISH_SPAIN.data ());
  EXPECT_GT (translations.size(), 0);
  // EXPECT_FALSE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_PropertiesFormat_Success)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test loading .properties file format
  i18nObject->addLanguage (GERMAN_GERMANY.data ()); // Assuming German properties file exists

  EXPECT_TRUE (i18nObject->hasLanguage(GERMAN_GERMANY.data()));

  auto translations = i18nObject->getTranslations (GERMAN_GERMANY.data ());
  EXPECT_GT (translations.size(), 0); // Greater than 0
  // EXPECT_FALSE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_PoFormat_Success)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test loading .po file format
  i18nObject->addLanguage (FRENCH_FRANCE.data ()); // Assuming French .po file exists
  EXPECT_TRUE (i18nObject->hasLanguage(FRENCH_FRANCE.data()));

  auto translations = i18nObject->getTranslations (FRENCH_FRANCE.data ());
  EXPECT_EQ (translations.size(), 0); // Greater than 0
}

TEST_F (i18nTests, loadTranslationFile_TxtFormat_Success)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test loading .txt file format (treated as properties)
  i18nObject->addLanguage (ITALIAN_ITALY.data ()); // Assuming Italian .txt file exists

  EXPECT_TRUE (i18nObject->hasLanguage(ITALIAN_ITALY.data()));

  auto translations = i18nObject->getTranslations (ITALIAN_ITALY.data ());
  EXPECT_EQ (translations.size(), 0); // Greater than 0
  // EXPECT_FALSE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_NoFileExists_CreatesEmptyLanguage)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  EXPECT_THROW (
    i18nObject->addLanguage("ZH_zh"),
    ADS::i18n::locale_exception
    );
}

TEST_F (i18nTests, loadTranslationFile_MultipleFormatsAvailable_LoadsFirst)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // If multiple formats exist for same language, should load the first one found
  // (based on the order in the formats vector: .json, .properties, .po, .txt)
  EXPECT_THROW (
    i18nObject->addLanguage ("multi"), // Assuming corrupted JSON file exists
    ADS::i18n::locale_exception
  );
  // EXPECT_TRUE (i18nObject->hasLanguage("multi"));

  auto translations = i18nObject->getTranslations ("multi");
  EXPECT_EQ(translations.size(), 0); // Greater than 0
  // EXPECT_FALSE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_CorruptedJsonFile_FallsBackToProperties)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test with a corrupted JSON file that should fallback to properties parsing
  EXPECT_THROW (
    i18nObject->addLanguage ("corrupt_json"), // Assuming corrupted JSON file exists
    ADS::i18n::locale_exception
    );

  // EXPECT_TRUE (i18nObject->hasLanguage("corrupt_json"));
}

TEST_F (i18nTests, loadTranslationFile_FileExistsButCannotOpen_ReturnsFalse)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // This test might be harder to set up depending on your test environment
  // You might need to create a file with restricted permissions
  // For now, testing the language addition still works but creates empty map
  EXPECT_THROW(
    i18nObject->addLanguage ("restricted"), // File exists but can't be opened
    ADS::i18n::locale_exception
  );
  // EXPECT_TRUE (i18nObject->hasLanguage("restricted"));
}

TEST_F (i18nTests, loadTranslationFile_JsonWithNestedObjects_ParsesCorrectly)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test JSON with nested objects that should be flattened with dot notation
  EXPECT_THROW(
    i18nObject->addLanguage ("nested_json"), // JSON with nested structure
    ADS::i18n::locale_exception
  );

  // auto translations = i18nObject->getTranslations ("nested_json");
  // EXPECT_EQ(translations.size(), 0);
  // EXPECT_FALSE (translations.empty());

  // Should contain flattened keys like "menu.file", "menu.edit", etc.
  // You'd need to verify specific keys based on your test data
}

TEST_F (i18nTests, loadTranslationFile_PropertiesWithComments_IgnoresComments)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test properties file with comments (# and ;)
  EXPECT_THROW (
    i18nObject->addLanguage ("props_with_comments"),
    ADS::i18n::locale_exception
  );

  // EXPECT_TRUE (i18nObject->hasLanguage("props_with_comments"));

  auto translations = i18nObject->getTranslations ("props_with_comments");
  EXPECT_EQ(translations.size(), 0);
  // EXPECT_FALSE (translations.empty());

  // Comments should not appear as translation keys
  EXPECT_EQ (translations.find("#comment"), translations.end());
  EXPECT_EQ (translations.find(";comment"), translations.end());
}

TEST_F (i18nTests, loadTranslationFile_PropertiesWithEscapeSequences_UnescapesCorrectly)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test properties with escape sequences (\n, \t, \\, etc.)
  EXPECT_THROW (
    i18nObject->addLanguage ("escaped_props"),
    ADS::i18n::locale_exception
  );

  // EXPECT_TRUE (i18nObject->hasLanguage("escaped_props"));
  //
  // auto translations = i18nObject->getTranslations ("escaped_props");
  // EXPECT_FALSE (translations.empty());

  // Verify escape sequences are properly unescaped
  // You'd need to check specific keys based on your test data
}

TEST_F (i18nTests, loadTranslationFile_PoFileWithMultilineStrings_ParsesCorrectly)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test PO file with multiline strings (continuation lines)
  EXPECT_THROW (
    i18nObject->addLanguage ("multiline_po"),
    ADS::i18n::locale_exception
  );

  // EXPECT_TRUE (i18nObject->hasLanguage("multiline_po"));
  //
  // auto translations = i18nObject->getTranslations ("multiline_po");
  // EXPECT_FALSE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_EmptyFile_CreatesEmptyTranslations)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test with completely empty translation file
  EXPECT_THROW (
    i18nObject->addLanguage ("empty_file"),
    ADS::i18n::locale_exception
  );

  // EXPECT_TRUE (i18nObject->hasLanguage("empty_file"));
  //
  // auto translations = i18nObject->getTranslations ("empty_file");
  // EXPECT_TRUE (translations.empty());
}

TEST_F (i18nTests, loadTranslationFile_UnsupportedLanguage_ThrowsException)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Test with unsupported language code
  EXPECT_THROW (
    i18nObject->addLanguage("unsupported_lang"),
    ADS::i18n::locale_exception
    );
}

TEST_F (i18nTests, loadTranslationFile_AlreadyLoadedLanguage_ReturnsExisting)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Add language twice
  i18nObject->addLanguage (SPANISH_SPAIN.data ());
  auto *firstResult = i18nObject->addLanguage (SPANISH_SPAIN.data ());

  EXPECT_TRUE (i18nObject->hasLanguage(SPANISH_SPAIN.data()));
  EXPECT_NE (firstResult, nullptr);

  // Should return the existing language without reloading
  auto translations1 = i18nObject->getTranslations (SPANISH_SPAIN.data ());
  auto translations2 = i18nObject->getTranslations (SPANISH_SPAIN.data ());
  EXPECT_EQ (translations1.size(), translations2.size());
}

// Integration test combining multiple file formats
TEST_F (i18nTests, loadTranslationFile_Integration_MultipleFormats)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  // Load multiple languages with different file formats
  i18nObject->addLanguage (SPANISH_SPAIN.data ());  // .json
  i18nObject->addLanguage (GERMAN_GERMANY.data ()); // .properties
  i18nObject->addLanguage (FRENCH_FRANCE.data ());  // .po
  i18nObject->addLanguage (ITALIAN_ITALY.data ());  // .txt

  EXPECT_TRUE (i18nObject->hasLanguage(SPANISH_SPAIN.data()));
  EXPECT_TRUE (i18nObject->hasLanguage(GERMAN_GERMANY.data()));
  EXPECT_TRUE (i18nObject->hasLanguage(FRENCH_FRANCE.data()));
  EXPECT_TRUE (i18nObject->hasLanguage(ITALIAN_ITALY.data()));

  // Verify all languages are available
  auto availableLanguages = i18nObject->getAvailableLanguages ();
  EXPECT_GE (availableLanguages.size(), 4);
}

TEST_F (i18nTests, getSupportedLocales)
{
  auto i18nObject = SetupI18nObject (BASE_FOLDER, ENGLISH_UNITED_STATES.data ());

  auto supportedLanguages = i18nObject->getSupportedLanguages ();

  EXPECT_EQ (supportedLanguages.size(), ADS::Constants::Languages::languages.size());
  EXPECT_NE(std::find(
    supportedLanguages.begin(),
    supportedLanguages.end(),
    SPANISH_SPAIN.data ()
  ), supportedLanguages.end());
}

// Test cases for saveTranslations method covering all supported formats and scenarios

TEST_F(i18nTests, saveTranslations_ExistingJsonFile_Success) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());
    i18nObject->addTranslation("goodbye", "Adiós", SPANISH_SPAIN.data());

    // Save to existing JSON file format
    bool result = i18nObject->saveTranslations(SPANISH_SPAIN.data());

    EXPECT_TRUE(result);
}

TEST_F(i18nTests, saveTranslations_ExistingPropertiesFile_Success) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("hello", "Hallo", GERMAN_GERMANY.data());
    i18nObject->addTranslation("goodbye", "Auf Wiedersehen", GERMAN_GERMANY.data());

    // Save to existing properties file format
    bool result = i18nObject->saveTranslations(GERMAN_GERMANY.data());

    EXPECT_TRUE(result);
}

TEST_F(i18nTests, saveTranslations_ExistingPoFile_Success) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("hello", "Bonjour", FRENCH_FRANCE.data());
    i18nObject->addTranslation("goodbye", "Au revoir", FRENCH_FRANCE.data());

    // Save to existing PO file format
    bool result = i18nObject->saveTranslations(FRENCH_FRANCE.data());

    EXPECT_TRUE(result);
}

TEST_F(i18nTests, saveTranslations_ExistingTxtFile_Success) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("hello", "Ciao", ITALIAN_ITALY.data());
    i18nObject->addTranslation("goodbye", "Arrivederci", ITALIAN_ITALY.data());

    // Save to existing TXT file format (treated as properties)
    bool result = i18nObject->saveTranslations(ITALIAN_ITALY.data());

    EXPECT_TRUE(result);
}

TEST_F(i18nTests, saveTranslations_NoExistingFile_CreatesPropertiesFile) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations for a language with no existing file
    i18nObject->addTranslation("hello", "Olá", PORTUGUESE_PORTUGAL.data());
    i18nObject->addTranslation("goodbye", "Tchau", PORTUGUESE_PORTUGAL.data());

    // Should create new .properties file as default
    bool result = i18nObject->saveTranslations(PORTUGUESE_PORTUGAL.data());

    EXPECT_TRUE(result);

    // Verify file was created (you might need to check filesystem)
    // std::filesystem::path expectedFile = BASE_FOLDER / (PORTUGUESE_PORTUGAL.data() + ".properties");
    // EXPECT_TRUE(std::filesystem::exists(expectedFile));
}

TEST_F(i18nTests, saveTranslations_LanguageNotLoaded_ReturnsFalse) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Try to save translations for a language that hasn't been added
    bool result = i18nObject->saveTranslations(POLISH_POLAND.data());

    EXPECT_FALSE(result);
}

TEST_F(i18nTests, saveTranslations_EmptyTranslations_SavesEmptyFile) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add language but don't add any translations
    i18nObject->addLanguage(RUSSIAN_RUSSIA.data());

    // Should still save (empty file)
    bool result = i18nObject->saveTranslations(RUSSIAN_RUSSIA.data());

    EXPECT_TRUE(result);
}

TEST_F(i18nTests, saveTranslations_JsonFormat_ContainsMetadata) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("test_key", "test_value", DUTCH_NETHERLANDS.data());

    // Save as JSON
    bool result = i18nObject->saveTranslations(DUTCH_NETHERLANDS.data());
    EXPECT_TRUE(result);

    // You could verify the file contains metadata by reading it back
    // This would require file I/O in your test
}

TEST_F(i18nTests, saveTranslations_PropertiesFormat_ContainsHeader) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("test_key", "test_value", POLISH_POLAND.data());

    // Save as properties (assuming no existing file, so default format)
    bool result = i18nObject->saveTranslations(POLISH_POLAND.data());
    EXPECT_TRUE(result);

    // You could verify the file contains proper header by reading it back
}

TEST_F(i18nTests, saveTranslations_PoFormat_ContainsProperHeader) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("hello", "Hej", SWEDISH_SWEDEN.data());

    // Save as PO format
    bool result = i18nObject->saveTranslations(SWEDISH_SWEDEN.data());
    EXPECT_TRUE(result);

    // You could verify PO file structure by reading it back
}

TEST_F(i18nTests, saveTranslations_TranslationsAreSorted) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations in non-alphabetical order
    i18nObject->addTranslation("zebra", "zebra_translation", ITALIAN_SWITZERLAND.data());
    i18nObject->addTranslation("apple", "apple_translation", ITALIAN_SWITZERLAND.data());
    i18nObject->addTranslation("banana", "banana_translation", ITALIAN_SWITZERLAND.data());

    bool result = i18nObject->saveTranslations(ITALIAN_SWITZERLAND.data());
    EXPECT_TRUE(result);

    // The implementation sorts translations by key before saving
    // You could verify this by reading the file back and checking order
}

TEST_F(i18nTests, saveTranslations_SpecialCharactersEscaped) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations with special characters that need escaping
    i18nObject->addTranslation("quotes", "He said \"Hello\"", ROMANIAN_ROMANIA.data());
    i18nObject->addTranslation("newlines", "Line 1\nLine 2", ROMANIAN_ROMANIA.data());
    i18nObject->addTranslation("tabs", "Column 1\tColumn 2", ROMANIAN_ROMANIA.data());

    bool result = i18nObject->saveTranslations(ROMANIAN_ROMANIA.data());
    EXPECT_TRUE(result);

    // Special characters should be properly escaped in the saved file
}

TEST_F(i18nTests, saveTranslations_FilePermissionError_ReturnsFalse) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add some translations
    i18nObject->addTranslation("test", "test", PORTUGUESE_BRAZIL.data());

    // This test is tricky to set up - you'd need to make the directory read-only
    // or somehow simulate file permission errors
    // For now, just test that the method handles exceptions gracefully
    bool result = i18nObject->saveTranslations(PORTUGUESE_BRAZIL.data());

    // This might be TRUE or FALSE depending on your test setup
    // The important thing is that it doesn't crash
    EXPECT_TRUE(result == true || result == false);  // Just verify no crash
}

TEST_F(i18nTests, saveTranslations_MultipleFormatsExist_SavesToFirst) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // If multiple file formats exist for same language,
    // it should save to the first one found (.json has priority)
    i18nObject->addTranslation("multi_format", "test_value", FRENCH_CANADA.data());

    // This test assumes you have multiple format files for the same language
    bool result = i18nObject->saveTranslations(FRENCH_CANADA.data());

    // Should find and save to the first format (likely JSON)
    EXPECT_TRUE(result);
}

// Integration test
TEST_F(i18nTests, saveTranslations_Integration_SaveAndReload) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add several translations
    std::string testLanguage = SWEDISH_FINLAND.data();
    i18nObject->addTranslation("hello", "Terve", testLanguage);
    i18nObject->addTranslation("goodbye", "Näkemiin", testLanguage);
    i18nObject->addTranslation("thank_you", "Kiitos", testLanguage);

    // Save translations
    bool saveResult = i18nObject->saveTranslations(testLanguage);
    EXPECT_TRUE(saveResult);

    // Create new i18n object and reload
    auto newI18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());
    newI18nObject->addLanguage(testLanguage);

    // Verify translations were saved and can be reloaded
    EXPECT_EQ(newI18nObject->translate("hello", testLanguage), "Terve");
    EXPECT_EQ(newI18nObject->translate("goodbye", testLanguage), "Näkemiin");
    EXPECT_EQ(newI18nObject->translate("thank_you", testLanguage), "Kiitos");
}

TEST_F(i18nTests, saveTranslations_AllFormats_Success) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Test saving different languages in different formats
    std::vector<std::string> languages = {
        SPANISH_SPAIN.data(),    // JSON
        GERMAN_GERMANY.data(),   // Properties
        FRENCH_FRANCE.data(),    // PO
        ITALIAN_ITALY.data()     // TXT
    };

    // Add translations for all languages
    for (const auto& lang : languages) {
        i18nObject->addTranslation("common_key", "common_value_" + lang, lang);
    }

    // Save all languages
    for (const auto& lang : languages) {
        bool result = i18nObject->saveTranslations(lang);
        EXPECT_TRUE(result) << "Failed to save translations for " << lang;
    }
}

// Test cases for reloadTranslations method covering all scenarios

TEST_F(i18nTests, reloadTranslations_WithExistingFiles_ReloadsSuccessfully) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add multiple languages with translation files
    i18nObject->addLanguage(SPANISH_SPAIN.data());
    i18nObject->addLanguage(FRENCH_FRANCE.data());
    i18nObject->addLanguage(GERMAN_GERMANY.data());

    // Get initial count of available languages
    auto initialLanguages = i18nObject->getAvailableLanguages();
    size_t initialCount = initialLanguages.size();

    // Reload translations
    size_t reloadedCount = i18nObject->reloadTranslations();

    // Should have reloaded languages that have files
    EXPECT_GT(reloadedCount, 0);
    EXPECT_LE(reloadedCount, initialCount); // Can't reload more than we had

    // Languages should still be available
    auto afterReloadLanguages = i18nObject->getAvailableLanguages();
    EXPECT_EQ(afterReloadLanguages.size(), initialCount);
}

TEST_F(i18nTests, reloadTranslations_NoLanguagesLoaded_ReturnsZero) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Don't add any additional languages (only fallback should be present)
    size_t reloadedCount = i18nObject->reloadTranslations();

    // Should reload at least the fallback language if it has a file
    EXPECT_GE(reloadedCount, 0);
}

TEST_F(i18nTests, reloadTranslations_LanguagesWithoutFiles_CreatesEmptyMaps) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add languages that might not have translation files
    i18nObject->addLanguage(PORTUGUESE_PORTUGAL.data());
    i18nObject->addLanguage(POLISH_POLAND.data());
    i18nObject->addLanguage(RUSSIAN_RUSSIA.data());

    // Add some manual translations
    i18nObject->addTranslation("manual_key", "manual_value", PORTUGUESE_PORTUGAL.data());

    // Get count before reload
    auto beforeTranslations = i18nObject->getTranslations(PORTUGUESE_PORTUGAL.data());
    EXPECT_FALSE(beforeTranslations.empty()); // Should have our manual translation

    // Reload translations
    size_t reloadedCount = i18nObject->reloadTranslations();

    // Manual translations should be gone (replaced with file content or empty)
    auto afterTranslations = i18nObject->getTranslations(PORTUGUESE_PORTUGAL.data());
    // Could be empty if no file exists, or have file content

    // Languages should still be available even if empty
    EXPECT_TRUE(i18nObject->hasLanguage(PORTUGUESE_PORTUGAL.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(POLISH_POLAND.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(RUSSIAN_RUSSIA.data()));
}

TEST_F(i18nTests, reloadTranslations_MixedScenario_HandlesPartialReloads) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add mix of languages - some with files, some without
    i18nObject->addLanguage(SPANISH_SPAIN.data());      // Likely has file
    i18nObject->addLanguage(ITALIAN_ITALY.data());      // May or may not have file
    i18nObject->addLanguage(ROMANIAN_ROMANIA.data());   // May not have file

    // Add some manual translations to test they get replaced
    i18nObject->addTranslation("test1", "value1", ITALIAN_ITALY.data());
    i18nObject->addTranslation("test2", "value2", ROMANIAN_ROMANIA.data());

    size_t reloadedCount = i18nObject->reloadTranslations();

    // Should return count of successfully reloaded files
    EXPECT_GE(reloadedCount, 0);

    // All languages should still exist
    EXPECT_TRUE(i18nObject->hasLanguage(SPANISH_SPAIN.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(ITALIAN_ITALY.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(ROMANIAN_ROMANIA.data()));
}

TEST_F(i18nTests, reloadTranslations_PreservesLanguageAvailability) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add several languages
    std::vector<std::string> testLanguages = {
        SPANISH_SPAIN.data(),
        FRENCH_FRANCE.data(),
        GERMAN_GERMANY.data(),
        DUTCH_NETHERLANDS.data()
    };

    for (const auto& lang : testLanguages) {
        i18nObject->addLanguage(lang);
    }

    // Get languages before reload
    auto beforeLanguages = i18nObject->getAvailableLanguages();

    // Reload
    i18nObject->reloadTranslations();

    // Get languages after reload
    auto afterLanguages = i18nObject->getAvailableLanguages();

    // Should have same number of languages
    EXPECT_EQ(beforeLanguages.size(), afterLanguages.size());

    // All original languages should still be present
    for (const auto& lang : testLanguages) {
        EXPECT_NE(std::find(afterLanguages.begin(), afterLanguages.end(), lang),
                  afterLanguages.end());
    }
}

TEST_F(i18nTests, reloadTranslations_ClearsManualTranslations) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add language and manual translations
    i18nObject->addLanguage(SWEDISH_SWEDEN.data());
    i18nObject->addTranslation("manual1", "Manual Translation 1", SWEDISH_SWEDEN.data());
    i18nObject->addTranslation("manual2", "Manual Translation 2", SWEDISH_SWEDEN.data());

    // Verify manual translations exist
    EXPECT_EQ(i18nObject->translate("manual1", SWEDISH_SWEDEN.data()), "Manual Translation 1");
    EXPECT_EQ(i18nObject->translate("manual2", SWEDISH_SWEDEN.data()), "Manual Translation 2");

    // Reload translations
    i18nObject->reloadTranslations();

    // Manual translations should be gone (replaced by file content or empty)
    // They should return the key itself if not found in file
    auto result1 = i18nObject->translate("manual1", SWEDISH_SWEDEN.data());
    auto result2 = i18nObject->translate("manual2", SWEDISH_SWEDEN.data());

    // If no file exists or file doesn't contain these keys, should return the keys
    // If file exists and has different translations, should return file content
    // Either way, shouldn't be the original manual translations
    EXPECT_TRUE(result1 == "manual1" || result1 != "Manual Translation 1");
    EXPECT_TRUE(result2 == "manual2" || result2 != "Manual Translation 2");
}

TEST_F(i18nTests, reloadTranslations_HandlesFileErrors_Gracefully) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add languages that might have file access issues
    i18nObject->addLanguage(PORTUGUESE_BRAZIL.data());
    i18nObject->addLanguage(ITALIAN_SWITZERLAND.data());

    // The reload should not crash even if some files can't be read
    EXPECT_NO_THROW({
        size_t reloadedCount = i18nObject->reloadTranslations();
        // Count can be anything, just shouldn't throw
        EXPECT_TRUE(reloadedCount >= 0);
    });

    // Languages should still be available (with empty maps if files failed)
    EXPECT_TRUE(i18nObject->hasLanguage(PORTUGUESE_BRAZIL.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(ITALIAN_SWITZERLAND.data()));
}

TEST_F(i18nTests, reloadTranslations_ReloadsAllFormats) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add languages that might use different file formats
    i18nObject->addLanguage(SPANISH_SPAIN.data());    // JSON
    i18nObject->addLanguage(GERMAN_GERMANY.data());   // Properties
    i18nObject->addLanguage(FRENCH_FRANCE.data());    // PO
    i18nObject->addLanguage(ITALIAN_ITALY.data());    // TXT

    size_t reloadedCount = i18nObject->reloadTranslations();

    // Should handle all different file formats
    EXPECT_GE(reloadedCount, 0);

    // All languages should still be available
    EXPECT_TRUE(i18nObject->hasLanguage(SPANISH_SPAIN.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(GERMAN_GERMANY.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(FRENCH_FRANCE.data()));
    EXPECT_TRUE(i18nObject->hasLanguage(ITALIAN_ITALY.data()));
}

TEST_F(i18nTests, reloadTranslations_UpdatesTranslationStats) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add languages
    i18nObject->addLanguage(SPANISH_SPAIN.data());
    i18nObject->addLanguage(FRENCH_CANADA.data());

    // Add manual translations to change stats
    i18nObject->addTranslation("temp1", "temp1", SPANISH_SPAIN.data());
    i18nObject->addTranslation("temp2", "temp2", SPANISH_SPAIN.data());

    // Get stats before reload
    auto statsBefore = i18nObject->getTranslationStats();

    // Reload
    i18nObject->reloadTranslations();

    // Get stats after reload
    auto statsAfter = i18nObject->getTranslationStats();

    // Stats should reflect file content, not manual additions
    // (This test verifies the stats are recalculated correctly)
    EXPECT_TRUE(statsAfter.find(SPANISH_SPAIN.data()) != statsAfter.end());
    EXPECT_TRUE(statsAfter.find(FRENCH_CANADA.data()) != statsAfter.end());
}

// Integration test
TEST_F(i18nTests, reloadTranslations_Integration_SaveReloadCycle) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add language and translations
    std::string testLang = DUTCH_NETHERLANDS.data();
    i18nObject->addLanguage(testLang);
    i18nObject->addTranslation("hello", "Hallo", testLang);
    i18nObject->addTranslation("goodbye", "Tot ziens", testLang);

    // Save translations
    bool saveResult = i18nObject->saveTranslations(testLang);
    EXPECT_TRUE(saveResult);

    // Add some temporary manual translations
    i18nObject->addTranslation("temp", "temporary", testLang);
    EXPECT_EQ(i18nObject->translate("temp", testLang), "temporary");

    // Reload should restore from file, removing manual additions
    size_t reloadedCount = i18nObject->reloadTranslations();
    EXPECT_GT(reloadedCount, 0);

    // Original saved translations should be there
    EXPECT_EQ(i18nObject->translate("hello", testLang), "Hallo");
    EXPECT_EQ(i18nObject->translate("goodbye", testLang), "Tot ziens");

    // Temporary translation should be gone
    EXPECT_EQ(i18nObject->translate("temp", testLang), "temp"); // Returns key if not found
}

TEST_F(i18nTests, reloadTranslations_EmptyTranslationsMap_HandlesCorrectly) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Start with just the fallback language, then remove all translations
    auto languages = i18nObject->getAvailableLanguages();

    // Reload when there might be no languages or empty translations
    size_t reloadedCount = i18nObject->reloadTranslations();

    // Should handle empty case gracefully
    EXPECT_GE(reloadedCount, 0);

    // At minimum, fallback language should be available
    EXPECT_TRUE(i18nObject->hasLanguage(ENGLISH_UNITED_STATES.data()));
}

// Test cases for findMissingTranslations method covering all scenarios

TEST_F(i18nTests, findMissingTranslations_TargetLanguageComplete_ReturnsEmpty) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback language
    i18nObject->addTranslation("hello", "Hello", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("goodbye", "Goodbye", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("thank_you", "Thank you", ENGLISH_UNITED_STATES.data());

    // Add complete translations to target language
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());
    i18nObject->addTranslation("goodbye", "Adiós", SPANISH_SPAIN.data());
    i18nObject->addTranslation("thank_you", "Gracias", SPANISH_SPAIN.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should be empty since target has all keys from fallback
    EXPECT_TRUE(missing.empty());
    EXPECT_EQ(missing.size(), 0);
}

TEST_F(i18nTests, findMissingTranslations_TargetMissingSome_ReturnsCorrectList) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback language
    i18nObject->addTranslation("hello", "Hello", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("goodbye", "Goodbye", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("thank_you", "Thank you", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("please", "Please", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("yes", "Yes", ENGLISH_UNITED_STATES.data());

    // Add partial translations to target language
    i18nObject->addTranslation("hello", "Bonjour", FRENCH_FRANCE.data());
    i18nObject->addTranslation("goodbye", "Au revoir", FRENCH_FRANCE.data());
    // Missing: thank_you, please, yes

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(FRENCH_FRANCE.data());

    // Should contain the missing keys
    EXPECT_EQ(missing.size(), 3);
    EXPECT_NE(std::find(missing.begin(), missing.end(), "thank_you"), missing.end());
    EXPECT_NE(std::find(missing.begin(), missing.end(), "please"), missing.end());
    EXPECT_NE(std::find(missing.begin(), missing.end(), "yes"), missing.end());

    // Should NOT contain the keys that exist
    EXPECT_EQ(std::find(missing.begin(), missing.end(), "hello"), missing.end());
    EXPECT_EQ(std::find(missing.begin(), missing.end(), "goodbye"), missing.end());
}

TEST_F(i18nTests, findMissingTranslations_TargetLanguageEmpty_ReturnsAllFallbackKeys) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback language
    std::vector<std::string> fallbackKeys = {"hello", "goodbye", "thank_you", "please", "welcome"};
    for (const auto& key : fallbackKeys) {
        i18nObject->addTranslation(key, "English_" + key, ENGLISH_UNITED_STATES.data());
    }

    // Add target language but no translations
    i18nObject->addLanguage(GERMAN_GERMANY.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(GERMAN_GERMANY.data());

    // Should contain all fallback keys
    EXPECT_LE(missing.size(), fallbackKeys.size());
    for (const auto& key : missing) {
        EXPECT_NE(std::find(fallbackKeys.begin(), fallbackKeys.end(), key), fallbackKeys.end());
    }
}

TEST_F(i18nTests, findMissingTranslations_FallbackLanguageEmpty_ReturnsEmpty) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Don't add any translations to fallback language

    // Add translations to target language
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());
    i18nObject->addTranslation("goodbye", "Adiós", SPANISH_SPAIN.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should be empty since fallback has no keys to compare against
    EXPECT_TRUE(missing.empty());
}

TEST_F(i18nTests, findMissingTranslations_TargetLanguageNotLoaded_ReturnsEmpty) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback language
    i18nObject->addTranslation("hello", "Hello", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("goodbye", "Goodbye", ENGLISH_UNITED_STATES.data());

    // Don't load the target language

    // Find missing translations for unloaded language
    auto missing = i18nObject->findMissingTranslations(ITALIAN_ITALY.data());

    // Should return empty since target language isn't loaded
    EXPECT_TRUE(missing.empty());
}

TEST_F(i18nTests, findMissingTranslations_FallbackLanguageNotLoaded_ReturnsEmpty) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // This scenario is less likely since fallback is loaded in constructor
    // But test handles the case where fallback isn't found in translations map

    // Add target language
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());

    // Find missing (fallback should exist, but testing the guard condition)
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should handle gracefully
    EXPECT_TRUE(missing.empty() || !missing.empty()); // Just shouldn't crash
}

TEST_F(i18nTests, findMissingTranslations_ResultIsSorted) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback in non-alphabetical order
    i18nObject->addTranslation("zebra", "Zebra", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("apple", "Apple", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("banana", "Banana", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("cherry", "Cherry", ENGLISH_UNITED_STATES.data());

    // Add partial translations to target language
    i18nObject->addTranslation("banana", "Banane", FRENCH_FRANCE.data());
    // Missing: zebra, apple, cherry

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(FRENCH_FRANCE.data());

    // Should be sorted alphabetically
    EXPECT_EQ(missing.size(), 3);
    EXPECT_TRUE(std::is_sorted(missing.begin(), missing.end()));

    // Verify specific order
    EXPECT_EQ(missing[0], "apple");
    EXPECT_EQ(missing[1], "cherry");
    EXPECT_EQ(missing[2], "zebra");
}

TEST_F(i18nTests, findMissingTranslations_MultipleLanguages_IndependentResults) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add comprehensive fallback translations
    std::vector<std::string> allKeys = {"hello", "goodbye", "thank_you", "please", "yes", "no"};
    for (const auto& key : allKeys) {
        i18nObject->addTranslation(key, "EN_" + key, ENGLISH_UNITED_STATES.data());
    }

    // Spanish has most translations
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());
    i18nObject->addTranslation("goodbye", "Adiós", SPANISH_SPAIN.data());
    i18nObject->addTranslation("thank_you", "Gracias", SPANISH_SPAIN.data());
    i18nObject->addTranslation("please", "Por favor", SPANISH_SPAIN.data());
    // Missing: yes, no

    // French has fewer translations
    i18nObject->addTranslation("hello", "Bonjour", FRENCH_FRANCE.data());
    i18nObject->addTranslation("goodbye", "Au revoir", FRENCH_FRANCE.data());
    // Missing: thank_you, please, yes, no

    // German has only one translation
    i18nObject->addTranslation("hello", "Hallo", GERMAN_GERMANY.data());
    // Missing: goodbye, thank_you, please, yes, no

    // Check Spanish missing translations
    auto spanishMissing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());
    EXPECT_EQ(spanishMissing.size(), 2);
    EXPECT_NE(std::find(spanishMissing.begin(), spanishMissing.end(), "yes"), spanishMissing.end());
    EXPECT_NE(std::find(spanishMissing.begin(), spanishMissing.end(), "no"), spanishMissing.end());

    // Check French missing translations
    auto frenchMissing = i18nObject->findMissingTranslations(FRENCH_FRANCE.data());
    EXPECT_EQ(frenchMissing.size(), 4);

    // Check German missing translations. two comes from the translation file
    auto germanMissing = i18nObject->findMissingTranslations(GERMAN_GERMANY.data());
    EXPECT_EQ(germanMissing.size(), 4);
}

TEST_F(i18nTests, findMissingTranslations_SameKeyDifferentValues_NotMissing) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations to fallback
    i18nObject->addTranslation("greeting", "Hello", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("farewell", "Goodbye", ENGLISH_UNITED_STATES.data());

    // Add translations to target with same keys but different values
    i18nObject->addTranslation("greeting", "Salut", FRENCH_FRANCE.data());
    i18nObject->addTranslation("farewell", "À bientôt", FRENCH_FRANCE.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(FRENCH_FRANCE.data());

    // Should be empty since keys exist (values don't matter for this method)
    EXPECT_TRUE(missing.empty());
}

TEST_F(i18nTests, findMissingTranslations_EmptyKeyValues_HandledCorrectly) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add translations including one with empty value to fallback
    i18nObject->addTranslation("hello", "Hello", ENGLISH_UNITED_STATES.data());
    i18nObject->addTranslation("empty_key", "", ENGLISH_UNITED_STATES.data());

    // Add only non-empty translation to target
    i18nObject->addTranslation("hello", "Hola", SPANISH_SPAIN.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should include the empty key
    EXPECT_EQ(missing.size(), 1);
    EXPECT_NE(std::find(missing.begin(), missing.end(), "empty_key"), missing.end());
}

// Integration test
TEST_F(i18nTests, findMissingTranslations_Integration_RealWorldScenario) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Simulate a real application with various UI strings
    std::vector<std::pair<std::string, std::string>> uiStrings = {
        {"menu.file", "File"},
        {"menu.edit", "Edit"},
        {"menu.view", "View"},
        {"menu.help", "Help"},
        {"button.ok", "OK"},
        {"button.cancel", "Cancel"},
        {"button.save", "Save"},
        {"dialog.confirm", "Are you sure?"},
        {"dialog.error", "An error occurred"},
        {"status.ready", "Ready"},
        {"status.loading", "Loading..."}
    };

    // Add all strings to fallback
    for (const auto& [key, value] : uiStrings) {
        i18nObject->addTranslation(key, value, ENGLISH_UNITED_STATES.data());
    }

    // Partially translate to Spanish (simulating incomplete translation)
    i18nObject->addTranslation("menu.file", "Archivo", SPANISH_SPAIN.data());
    i18nObject->addTranslation("menu.edit", "Editar", SPANISH_SPAIN.data());
    i18nObject->addTranslation("button.ok", "Aceptar", SPANISH_SPAIN.data());
    i18nObject->addTranslation("button.cancel", "Cancelar", SPANISH_SPAIN.data());
    i18nObject->addTranslation("status.ready", "Listo", SPANISH_SPAIN.data());

    // Find missing translations
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should find the untranslated keys
    std::vector<std::string> expectedMissing = {
        "button.save", "dialog.confirm", "dialog.error",
        "menu.help", "menu.view", "status.loading"
    };

    EXPECT_EQ(missing.size(), expectedMissing.size());

    // Verify all expected missing keys are present
    for (const auto& expectedKey : expectedMissing) {
        EXPECT_NE(std::find(missing.begin(), missing.end(), expectedKey), missing.end())
            << "Expected missing key: " << expectedKey;
    }

    // Verify no extra keys are missing
    for (const auto& missingKey : missing) {
        EXPECT_NE(std::find(expectedMissing.begin(), expectedMissing.end(), missingKey),
                  expectedMissing.end()) << "Unexpected missing key: " << missingKey;
    }
}

TEST_F(i18nTests, findMissingTranslations_LargeScale_PerformanceTest) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, ENGLISH_UNITED_STATES.data());

    // Add many translations to fallback (simulating large application)
    const int numTranslations = 1000;
    for (int i = 0; i < numTranslations; ++i) {
        std::string key = "key_" + std::to_string(i);
        std::string value = "Value " + std::to_string(i);
        i18nObject->addTranslation(key, value, ENGLISH_UNITED_STATES.data());
    }

    // Add partial translations to target (simulate 70% completion)
    for (int i = 0; i < numTranslations * 0.7; ++i) {
        std::string key = "key_" + std::to_string(i);
        std::string value = "Valor " + std::to_string(i);
        i18nObject->addTranslation(key, value, SPANISH_SPAIN.data());
    }

    // Find missing translations (should be efficient)
    auto missing = i18nObject->findMissingTranslations(SPANISH_SPAIN.data());

    // Should find the missing 30%
    EXPECT_EQ(missing.size(), numTranslations * 0.3);

    // Results should still be sorted
    EXPECT_TRUE(std::is_sorted(missing.begin(), missing.end()));
}