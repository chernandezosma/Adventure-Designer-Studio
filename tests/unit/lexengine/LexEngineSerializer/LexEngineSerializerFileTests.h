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

#ifndef LEXENGINESERIALIZERFILETESTS_H
#define LEXENGINESERIALIZERFILETESTS_H

#include <gtest/gtest.h>
#include <filesystem>

/**
 * @brief Fixture that builds a temporary directory for each saveToFile/loadFromFile test.
 */
class LexEngineSerializerFileTests : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    std::filesystem::path testDir;
};

#endif // LEXENGINESERIALIZERFILETESTS_H
