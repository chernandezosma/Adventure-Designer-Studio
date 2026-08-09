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

#ifndef I18NTESTS_H
#define I18NTESTS_H

#include <gtest/gtest.h>
#include <filesystem>
#include <map>
#include <string>

/**
 * @brief Fixture that builds a temporary translation folder for each test.
 *
 * Forces LC_ALL=C for the duration of each test so i18n's system-locale
 * detection deterministically falls back to the constructor's fallback
 * language, regardless of the host machine's locale.
 */
class i18nTests : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    // Writes <lang>.json under the test's temp folder with the given raw content.
    void createJsonFile(const std::string& lang, const std::string& jsonContent);

    std::filesystem::path testDir;
    std::string baseFolder;

private:
    std::map<std::string, std::string> originalEnv;
};

#endif // I18NTESTS_H