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
 * @file EnvironmentTests.cpp
 * @brief Covers Environment::set() — the comment/order-preserving .env writer.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "env/env.h"

namespace fs = std::filesystem;

namespace {

    class EnvironmentSetTest : public ::testing::Test {
    protected:
        fs::path envFile;

        void SetUp() override
        {
            envFile = fs::temp_directory_path() /
                      ("ads_env_test_" +
                       std::to_string(::testing::UnitTest::GetInstance()->random_seed()) + ".env");
            write(
                "# Adventure Designer Studio test env\n"
                "DEBUG=true\n"
                "\n"
                "# Active languages\n"
                "LANGUAGES=es_ES,de_DE,en_US\n"
                "FONT_SIZE_BASE=14.0f\n");
        }

        void TearDown() override
        {
            std::error_code ec;
            fs::remove(envFile, ec);
        }

        void write(const std::string& contents) const
        {
            std::ofstream out(envFile, std::ios::trunc);
            out << contents;
        }

        std::string read() const
        {
            std::ifstream in(envFile);
            std::stringstream buf;
            buf << in.rdbuf();
            return buf.str();
        }
    };

    TEST_F(EnvironmentSetTest, SetNewKey_AppendsAndKeepsExistingContent)
    {
        {
            ADS::Environment env(envFile.string());
            ASSERT_TRUE(env.set("UI_LANGUAGE", "fr_FR"));
        }

        const std::string text = read();
        EXPECT_NE(text.find("# Adventure Designer Studio test env"), std::string::npos);
        EXPECT_NE(text.find("# Active languages"), std::string::npos);
        EXPECT_NE(text.find("LANGUAGES=es_ES,de_DE,en_US"), std::string::npos);
        EXPECT_NE(text.find("FONT_SIZE_BASE=14.0f"), std::string::npos);
        EXPECT_NE(text.find("UI_LANGUAGE=fr_FR"), std::string::npos);

        // Re-loading sees the new value.
        ADS::Environment reloaded(envFile.string());
        EXPECT_EQ(reloaded.getOrDefault("UI_LANGUAGE", ""), "fr_FR");
    }

    TEST_F(EnvironmentSetTest, SetExistingKey_ReplacesValueInPlace)
    {
        const std::string before = read();
        const auto linesBefore = std::count(before.begin(), before.end(), '\n');

        {
            ADS::Environment env(envFile.string());
            ASSERT_TRUE(env.set("LANGUAGES", "en_US"));
        }

        const std::string text = read();
        EXPECT_NE(text.find("LANGUAGES=en_US"), std::string::npos);
        EXPECT_EQ(text.find("es_ES,de_DE"), std::string::npos);
        // Comments preserved, no line added.
        EXPECT_NE(text.find("# Active languages"), std::string::npos);
        EXPECT_EQ(std::count(text.begin(), text.end(), '\n'), linesBefore);
    }

    TEST_F(EnvironmentSetTest, SetExistingKey_IsCaseInsensitive)
    {
        {
            ADS::Environment env(envFile.string());
            ASSERT_TRUE(env.set("languages", "it_IT"));
        }
        ADS::Environment reloaded(envFile.string());
        EXPECT_EQ(reloaded.getOrDefault("LANGUAGES", ""), "it_IT");
        // Still exactly one LANGUAGES line.
        const std::string text = read();
        size_t first = text.find("LANGUAGES=");
        EXPECT_NE(first, std::string::npos);
        EXPECT_EQ(text.find("LANGUAGES=", first + 1), std::string::npos);
    }

    TEST_F(EnvironmentSetTest, ValueWithSpace_IsQuotedOnWriteAndUnquotedOnRead)
    {
        {
            ADS::Environment env(envFile.string());
            ASSERT_TRUE(env.set("WINDOW_TITLE", "My Game"));
        }
        EXPECT_NE(read().find("WINDOW_TITLE=\"My Game\""), std::string::npos);

        ADS::Environment reloaded(envFile.string());
        EXPECT_EQ(reloaded.getOrDefault("WINDOW_TITLE", ""), "My Game");
    }

    TEST_F(EnvironmentSetTest, Open_TrimsWhitespaceAroundKeyAndValue)
    {
        // "KEY = value" / "KEY= value" must be equivalent to "KEY=value"; a
        // stray space in a path value must not survive.
        write("PROJECTS_DIR=  /home/x/ADS-Projects  \n"
              "  UI_LANGUAGE = es_ES\n");

        ADS::Environment env(envFile.string());
        EXPECT_EQ(env.getOrDefault("PROJECTS_DIR", ""), "/home/x/ADS-Projects");
        EXPECT_EQ(env.getOrDefault("UI_LANGUAGE", ""), "es_ES");
    }

    TEST_F(EnvironmentSetTest, Open_StripsMatchingSurroundingQuotes)
    {
        write("A=\"quoted value\"\n"
              "B='single quoted'\n"
              "C=\"unbalanced\n");

        ADS::Environment env(envFile.string());
        EXPECT_EQ(env.getOrDefault("A", ""), "quoted value");
        EXPECT_EQ(env.getOrDefault("B", ""), "single quoted");
        EXPECT_EQ(env.getOrDefault("C", ""), "\"unbalanced");
    }

} // namespace
