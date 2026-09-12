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
 * @file PathServiceTests.cpp
 * @brief Unit tests for Core::PathService (slugify, path building, roots).
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "Core/PathService.h"

namespace fs = std::filesystem;
using ADS::Core::PathService;

TEST(PathService, Slugify_SpacesAndCaseBecomeKebab)
{
    EXPECT_EQ(PathService::slugify("The Forgotten Crypt"), "the-forgotten-crypt");
}

TEST(PathService, Slugify_IllegalCharsCollapseToSingleDash)
{
    EXPECT_EQ(PathService::slugify("a / b : c ? d"), "a-b-c-d");
    EXPECT_EQ(PathService::slugify("hello___world"), "hello-world");
    EXPECT_EQ(PathService::slugify("--trim--me--"), "trim-me");
}

TEST(PathService, Slugify_NonAsciiIsDropped)
{
    // "Café del Mar" -> the accented byte(s) become a separator run
    EXPECT_EQ(PathService::slugify("Café del Mar"), "caf-del-mar");
}

TEST(PathService, Slugify_EmptyOrPunctuationOnlyFallsBackToProject)
{
    EXPECT_EQ(PathService::slugify(""), "project");
    EXPECT_EQ(PathService::slugify("   ***   "), "project");
}

TEST(PathService, Slugify_WindowsReservedNamesArePrefixed)
{
    EXPECT_EQ(PathService::slugify("CON"), "_con");
    EXPECT_EQ(PathService::slugify("com1"), "_com1");
    EXPECT_EQ(PathService::slugify("LPT9"), "_lpt9");
    // Not reserved: has more than the device name.
    EXPECT_EQ(PathService::slugify("console"), "console");
}

TEST(PathService, Slugify_LengthIsCapped)
{
    const std::string huge(500, 'a');
    EXPECT_LE(PathService::slugify(huge).size(), 64u);
}

TEST(PathService, PathFromUtf8_RoundTripsAscii)
{
    const std::string in = "/tmp/ads-projects/my-game";
    EXPECT_EQ(PathService::pathFromUtf8(in).generic_string(), in);
}

TEST(PathService, ProjectsRoot_OverrideWins)
{
    const fs::path root = PathService::projectsRoot("/data/games");
    EXPECT_EQ(root, fs::path("/data/games"));
}

TEST(PathService, ProjectsRoot_TrimsSurroundingWhitespace)
{
    // A ".env" line like "PROJECTS_DIR= /data/games" must not yield a path
    // whose first component is a space.
    const fs::path root = PathService::projectsRoot("  /data/games\t");
    EXPECT_EQ(root, fs::path("/data/games"));
    EXPECT_TRUE(root.is_absolute());
}

TEST(PathService, ProjectsRoot_ExpandsLeadingTilde)
{
    const fs::path home = PathService::homeDir();
    EXPECT_EQ(PathService::projectsRoot("~"), home);
    EXPECT_EQ(PathService::projectsRoot(" ~/ADS-Projects "), home / "ADS-Projects");
}

TEST(PathService, ExpandUser_BlankInputIsEmpty)
{
    EXPECT_TRUE(PathService::expandUser("   ").empty());
    EXPECT_TRUE(PathService::expandUser("").empty());
}

TEST(PathService, ProjectsRoot_DefaultEndsWithAdsProjects)
{
    const fs::path root = PathService::projectsRoot();
    EXPECT_TRUE(root.is_absolute());
    EXPECT_EQ(root.filename(), "ADS-Projects");
}

TEST(PathService, ProjectFile_ComposesRootSlugSlugAds)
{
    const fs::path file = PathService::projectFile("/data/games", "My Game");
    EXPECT_EQ(file, fs::path("/data/games/my-game/my-game.ads"));
}

TEST(PathService, HomeDir_IsNonEmptyAbsolutePath)
{
    const fs::path home = PathService::homeDir();
    EXPECT_FALSE(home.empty());
    EXPECT_TRUE(home.is_absolute());
}

TEST(PathService, ProjectFolderExists_FalseForMissing_TrueForNonEmpty)
{
    const fs::path base = fs::temp_directory_path() /
        ("ads_ps_folder_" + std::to_string(::testing::UnitTest::GetInstance()->random_seed()));
    fs::remove_all(base);

    EXPECT_FALSE(PathService::projectFolderExists(base));

    fs::create_directories(base);
    EXPECT_FALSE(PathService::projectFolderExists(base)); // empty dir is free to use

    std::ofstream(base / "x.ads") << "{}";
    EXPECT_TRUE(PathService::projectFolderExists(base));

    fs::remove_all(base);
}

TEST(PathService, MaxPathLength_ReturnsPositiveValue)
{
    // Cross-platform sanity check: whatever this OS/config resolves to,
    // it must be a plausible, non-zero path budget.
    EXPECT_GT(PathService::maxPathLength(), 0u);
}

TEST(PathService, MaxPathLength_IsStableAcrossCalls)
{
    // Result is cached (function-local static) — repeated calls must agree.
    EXPECT_EQ(PathService::maxPathLength(), PathService::maxPathLength());
}

TEST(PathService, IsPathLengthValid_ShortPathIsValid)
{
    EXPECT_TRUE(PathService::isPathLengthValid(
        PathService::pathFromUtf8("short/relative/path.ads")));
}

TEST(PathService, IsPathLengthValid_OverLongPathIsRejected)
{
    // Build a path guaranteed to exceed maxPathLength() regardless of
    // platform (a component count well beyond even the Windows
    // long-path ceiling).
    std::string longPath = "root";
    while (longPath.size() <= PathService::maxPathLength()) {
        longPath += "/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    }
    EXPECT_FALSE(PathService::isPathLengthValid(PathService::pathFromUtf8(longPath)));
}

TEST(PathService, IsPathLengthValid_EmptyPathIsValid)
{
    EXPECT_TRUE(PathService::isPathLengthValid(fs::path{}));
}

#ifdef _WIN32
TEST(PathService, MaxPathLength_WindowsIsClassicOrLongPathValue)
{
    // Whatever the registry says on this machine, the resolved value must
    // be exactly one of the two known Windows ceilings — never an
    // arbitrary third number, guarding against a typo in the constants.
    const std::size_t value = PathService::maxPathLength();
    EXPECT_TRUE(value == 260 || value == 32767);
}
#else
TEST(PathService, MaxPathLength_PosixIsAtLeastPathMaxFallback)
{
    // On Linux, pathconf() may exceed 4096 for some mounts; on macOS it is
    // fixed at 1024. Either way it must be >= the smallest known POSIX floor.
    EXPECT_GE(PathService::maxPathLength(), 1024u);
}
#endif
