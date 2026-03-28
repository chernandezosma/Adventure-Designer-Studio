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

#ifndef ADS_TYPES_COLOR_H
#define ADS_TYPES_COLOR_H

/**
 * @file Color.h
 * @brief Portable RGBA color type with no external dependencies
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

namespace ADS::Types {

    /**
     * @brief Portable RGBA color type
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Stores red, green, blue, and alpha channel values in the [0.0, 1.0] range.
     * This type has no dependency on ImGui or any other rendering library and is
     * safe for use in DataObjects that will be serialised to disk.
     */
    struct Color {
        float r = 1.0f; ///< Red channel [0.0, 1.0]
        float g = 1.0f; ///< Green channel [0.0, 1.0]
        float b = 1.0f; ///< Blue channel [0.0, 1.0]
        float a = 1.0f; ///< Alpha channel [0.0, 1.0]

        /**
         * @brief Default constructor — opaque white
         */
        Color() = default;

        /**
         * @brief Construct from explicit RGBA components
         *
         * @param r Red channel
         * @param g Green channel
         * @param b Blue channel
         * @param a Alpha channel (default 1.0)
         */
        Color(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}

        /**
         * @brief Check equality with another color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param other The color to compare against
         * @return bool True if all four channels are equal
         */
        bool operator==(const Color& other) const {
            return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        /**
         * @brief Check inequality with another color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param other The color to compare against
         * @return bool True if any channel differs
         */
        bool operator!=(const Color& other) const {
            return !(*this == other);
        }
    };

} // namespace ADS::Types

#endif // ADS_TYPES_COLOR_H
