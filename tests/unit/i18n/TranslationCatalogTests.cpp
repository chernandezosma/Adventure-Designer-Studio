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
 * @brief Guards that every locale JSON under public/translations/core carries
 *        the exact same set of (flattened) keys.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * The IDE looks strings up with i18n::_t("GROUP.KEY"); a key missing from one
 * locale silently falls through to the fallback language (or the raw key). Any
 * feature that adds a translation key must add it to all seven files — this
 * test fails loudly when one is forgotten.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using nlohmann::json;

namespace {

    constexpr std::array<const char*, 7> kLocales{
        "de_DE", "en_US", "es_ES", "fr_FR", "it_IT", "pt_PT", "ru_RU"};

    fs::path catalogDir()
    {
        return fs::path(PROJECT_ROOT) / "public" / "translations" / "core";
    }

    /// Recursively collect dotted key paths for every leaf (non-object) value,
    /// mirroring i18n::parseJsonContent's flattening.
    void collectKeys(const json& node, const std::string& prefix, std::set<std::string>& out)
    {
        for (auto it = node.begin(); it != node.end(); ++it) {
            const std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            if (it.value().is_object()) {
                collectKeys(it.value(), key, out);
            } else {
                out.insert(key);
            }
        }
    }

    std::set<std::string> keysOf(const std::string& locale)
    {
        const fs::path file = catalogDir() / (locale + ".json");
        std::ifstream in(file);
        EXPECT_TRUE(in.is_open()) << "cannot open " << file;
        std::stringstream buf;
        buf << in.rdbuf();
        std::set<std::string> keys;
        collectKeys(json::parse(buf.str()), "", keys);
        return keys;
    }

    std::string join(const std::set<std::string>& s)
    {
        std::string r;
        for (const auto& k : s) { r += "\n  "; r += k; }
        return r;
    }

} // namespace

TEST(TranslationCatalogTests, EveryLocaleFileExists)
{
    for (const char* locale : kLocales) {
        const fs::path file = catalogDir() / (std::string(locale) + ".json");
        EXPECT_TRUE(fs::exists(file)) << "missing locale file: " << file;
    }
}

TEST(TranslationCatalogTests, AllLocalesShareTheSameKeySet)
{
    const std::set<std::string> reference = keysOf("en_US");
    ASSERT_FALSE(reference.empty());

    for (const char* locale : kLocales) {
        if (std::string(locale) == "en_US") continue;
        const std::set<std::string> keys = keysOf(locale);

        std::set<std::string> missing;
        std::set_difference(reference.begin(), reference.end(),
                            keys.begin(), keys.end(),
                            std::inserter(missing, missing.begin()));

        std::set<std::string> extra;
        std::set_difference(keys.begin(), keys.end(),
                            reference.begin(), reference.end(),
                            std::inserter(extra, extra.begin()));

        EXPECT_TRUE(missing.empty())
            << locale << ".json is missing keys present in en_US.json:" << join(missing);
        EXPECT_TRUE(extra.empty())
            << locale << ".json has keys absent from en_US.json:" << join(extra);
    }
}
