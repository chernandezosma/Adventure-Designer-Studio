#include <gtest/gtest.h>
#include <memory>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Include your original i18n header
#include "i18n/i18n.h"  // Adjust path as needed

using namespace ADS::i18n;
using namespace std;
namespace fs = std::filesystem;

// Mock implementation for Constants::Languages (only if needed)
namespace ADS::i18n::Constants::Languages {
    bool isLanguageSupported(const string& lang) {
        return lang == "en_US" || lang == "es_ES" || lang == "fr_FR" || lang == "de_DE";
    }
    
    string getLanguageName(const string& lang) {
        if (lang == "en_US") return "English";
        if (lang == "es_ES") return "Spanish";
        if (lang == "fr_FR") return "French";
        if (lang == "de_DE") return "German";
        return "";
    }
    
    string normalizePlatformLocale(const string& locale) {
        if (locale.find("en_US") != string::npos) return "en";
        if (locale.find("es_ES") != string::npos) return "es";
        if (locale.find("fr_FR") != string::npos) return "fr";
        if (locale.find("de_DE") != string::npos) return "de";
        return "";
    }
    
    vector<string> getSupportedLocales() {
        return {"en_US", "es_ES", "fr_FR", "de_DE"};
    }
}

    // Simple test fixture class (minimal)
    void I18nTests::SetUp() override {
        // Create temporary test directory
        testDir = fs::temp_directory_path() / "translations";
        this->createTestDirectories(testDir);
        BASE_FOLDER = testDir.string();
        createTestFiles();
    }

    I18nTests::createTestDirectories(string testDir) {
        if (!fs::exists(testDir)) {
            fs::create_directories(testDir);
        }




    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    void createTestFiles() {
        // Create Spanish properties file
        ofstream file(testDir / "es.properties");
        file << "hello=Hola\n";
        file << "goodbye=Adiós\n"; 
        file << "welcome=Bienvenido {name}\n";
        file.close();

        // Create English properties file
        ofstream enFile(testDir / "en.properties");
        enFile << "hello=Hello\n";
        enFile << "goodbye=Goodbye\n";
        enFile << "welcome=Welcome {name}\n";
        enFile.close();
    }

    // Helper methods
    std::unique_ptr<i18n> SetupI18nObject(const std::string& baseFolder, const char* fallback) {
        return std::make_unique<i18n>(baseFolder, std::string(fallback));
    }

    void verifyI18nObject(const std::unique_ptr<i18n>& obj) {
        EXPECT_NE(obj, nullptr);
        EXPECT_FALSE(obj->getCurrentLocale().locale.empty());
        EXPECT_TRUE(obj->getCurrentLocale().isValid());
    }

    fs::path testDir;
    std::string BASE_FOLDER;
};

// Now your tests using the original i18n class
TEST_F(I18nTests, Success_Object_Initialization) {
    std::unique_ptr<i18n> i18nObject = SetupI18nObject(
        BASE_FOLDER,
        "es_ES"  // or Constants::Languages::SPANISH_SPAIN.data() if you have it
    );
    verifyI18nObject(i18nObject);
}

TEST_F(I18nTests, BasicTranslation) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    
    string result = i18nObject->translate("hello");
    EXPECT_EQ(result, "Hello");
    
    result = i18nObject->translate("hello", "es");
    EXPECT_EQ(result, "Hola");
}

TEST_F(I18nTests, SetLocale) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    
    EXPECT_NO_THROW(i18nObject->setLocale("es"));
    EXPECT_EQ(i18nObject->getCurrentLocale().locale, "es");
}

TEST_F(I18nTests, TranslationWithParameters) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    
    unordered_map<string, string> params = {{"name", "John"}};
    string result = i18nObject->translateWithParams("welcome", params);
    EXPECT_EQ(result, "Welcome John");
    
    result = i18nObject->translateWithParams("welcome", params, "es");
    EXPECT_EQ(result, "Bienvenido John");
}

TEST_F(I18nTests, AddLanguage) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    
    EXPECT_NO_THROW(i18nObject->addLanguage("fr"));
    EXPECT_TRUE(i18nObject->hasLanguage("fr"));
}

TEST_F(I18nTests, InvalidLocale) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    
    EXPECT_THROW(i18nObject->setLocale("invalid"), locale_exception);
}

TEST_F(I18nTests, GetAvailableLanguages) {
    auto i18nObject = SetupI18nObject(BASE_FOLDER, "en");
    i18nObject->addLanguage("es");
    
    vector<string> languages = i18nObject->getAvailableLanguages();
    EXPECT_FALSE(languages.empty());
    EXPECT_TRUE(find(languages.begin(), languages.end(), "en") != languages.end());
    EXPECT_TRUE(find(languages.begin(), languages.end(), "es") != languages.end());
}

// Add more tests as needed...