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

#ifndef ADS_PROPERTY_TYPE_H
#define ADS_PROPERTY_TYPE_H

namespace ADS::Inspector {
    /**
     * @brief Enumeration of supported property types
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Defines all the property types that can be inspected and edited
     * in the property inspector panel.
     */
    enum class PropertyType {
        Unknown,    ///< Unknown or unsupported type
        Bool,       ///< Boolean value (true/false)
        Int,        ///< Integer value
        Float,      ///< Floating-point value
        String,     ///< Text string
        Color,      ///< RGBA color (ImVec4)
        Vector2,    ///< 2D vector (ImVec2)
        Enum        ///< Enumeration (selection from predefined values)
    };

    /**
     * @brief Get the display name of a property type
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param type The property type
     * @return const char* Human-readable name of the type
     */
    inline const char* getPropertyTypeName(PropertyType type) {
        switch (type) {
            case PropertyType::Bool:    return "Bool";
            case PropertyType::Int:     return "Int";
            case PropertyType::Float:   return "Float";
            case PropertyType::String:  return "String";
            case PropertyType::Color:   return "Color";
            case PropertyType::Vector2: return "Vector2";
            case PropertyType::Enum:    return "Enum";
            default:                    return "Unknown";
        }
    }
}

#endif //ADS_PROPERTY_TYPE_H