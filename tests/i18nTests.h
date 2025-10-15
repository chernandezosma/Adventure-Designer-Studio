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

#ifndef I18NTESTS_H
#define I18NTESTS_H

#include <gtest/gtest.h>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>

class i18nTests : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    // Helper methods
    void createPropertiesFile(const std::string& lang, const std::unordered_map<std::string, std::string>& translations);
    void createJsonFile(const std::string& lang, const std::string& jsonContent);
    void createPoFile(const std::string& lang, const std::unordered_map<std::string, std::string>& translations);
    void createMalformedFile(const std::string& name, const std::string& content);
    void createTestFiles();
    void createTestDirectories(const std::string& testDir);

    std::filesystem::path testDir;
    std::string BASE_FOLDER;
};

class I18nEnvironment {
protected:
    void setLocaleEnvironment(const std::string& locale);
    void clearLocaleEnvironment();

private:
    std::map<std::string, std::string> originalEnv;

public:
    void saveOriginalEnv();
    void restoreOriginalEnv();
    void setEnvVar(const std::string& name, const std::string& value);
};

class MockConstants {
public:
    static bool isLanguageSupported(const std::string& lang);
    static std::string getLanguageName(const std::string& lang);
    static std::string normalizePlatformLocale(const std::string& locale);
    static std::vector<std::string> getSupportedLocales();
};

// Helper functions
template<typename Container, typename T>
bool contains(const Container& container, const T& value);

bool stringContains(const std::string& haystack, const std::string& needle);

#endif // I18NTESTS_H