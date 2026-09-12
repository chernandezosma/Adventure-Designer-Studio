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

#ifndef ADS_CONSTANTS_IMAGE_FORMATS_H
#define ADS_CONSTANTS_IMAGE_FORMATS_H

/**
 * @file image_formats.h
 * @brief Single source of truth for the raster image formats the editor accepts
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * Every image / avatar field (Scene background, Character portrait + avatar,
 * Item icon) and the inspector thumbnail preview draw their allowed extensions
 * and their content check from here. To add a new format: add its extension to
 * supportedExtensions() AND its magic-number test to isSupportedImageFile(),
 * and enable the matching `sdl3-image` feature in `vcpkg.json`.
 */

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

namespace ADS::Constants::ImageFormats {

    /**
     * @brief Extensions (no dot, lower-case) the file dialog offers for images
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * PNG (.png), JPEG (.jpg / .jpeg) and WebP (.webp). The list is returned by
     * value so callers such as PropertyConstraints::filePath() can take it.
     *
     * @return std::vector<std::string> Accepted image extensions
     */
    inline std::vector<std::string> supportedExtensions()
    {
        return {"png", "jpg", "jpeg", "webp"};
    }

    /**
     * @brief Check a file's *content* is a PNG, JPEG or WebP image
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Reads the leading bytes and matches the format's magic number, so a file
     * that merely carries an image extension (a PDF renamed to `.png`, say) is
     * rejected. A file that cannot be opened is also rejected.
     *
     * @param path Filesystem path to inspect
     * @return bool true only when the bytes are a supported image format
     */
    inline bool isSupportedImageFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return false;
        }

        std::array<unsigned char, 12> head{};
        file.read(reinterpret_cast<char*>(head.data()),
                  static_cast<std::streamsize>(head.size()));
        const std::size_t read = static_cast<std::size_t>(file.gcount());

        // PNG: 89 50 4E 47 0D 0A 1A 0A
        static constexpr std::array<unsigned char, 8> kPng{
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        if (read >= kPng.size() &&
            std::equal(kPng.begin(), kPng.end(), head.begin())) {
            return true;
        }

        // JPEG: FF D8 FF
        if (read >= 3 && head[0] == 0xFF && head[1] == 0xD8 && head[2] == 0xFF) {
            return true;
        }

        // WebP: "RIFF" .... "WEBP"
        if (read >= 12 &&
            head[0] == 'R' && head[1] == 'I' && head[2] == 'F' && head[3] == 'F' &&
            head[8] == 'W' && head[9] == 'E' && head[10] == 'B' && head[11] == 'P') {
            return true;
        }

        return false;
    }

} // namespace ADS::Constants::ImageFormats

#endif // ADS_CONSTANTS_IMAGE_FORMATS_H
