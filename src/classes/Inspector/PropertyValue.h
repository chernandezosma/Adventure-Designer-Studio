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

#ifndef ADS_PROPERTY_VALUE_H
#define ADS_PROPERTY_VALUE_H

#include <variant>
#include <string>
#include "imgui.h"
#include "PropertyType.h"

namespace ADS::Inspector {
    /**
     * @brief Type alias for enum value storage
     *
     * Stores the selected index and available options for enum properties.
     */
    struct EnumValue {
        int selectedIndex = 0;
        std::vector<std::string> options;

        EnumValue() = default;
        EnumValue(int index, std::vector<std::string> opts)
            : selectedIndex(index), options(std::move(opts)) {}
    };

    /**
     * @brief Type-safe container for property values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Uses std::variant to provide type-safe storage for all supported
     * property value types. This allows the inspector system to handle
     * different property types uniformly while maintaining type safety.
     */
    using PropertyValue = std::variant<
        std::monostate,     ///< Empty/null state
        bool,               ///< Boolean value
        int,                ///< Integer value
        float,              ///< Floating-point value
        std::string,        ///< Text string
        ImVec4,             ///< RGBA color
        ImVec2,             ///< 2D vector
        EnumValue           ///< Enumeration value
    >;

    /**
     * @brief Get the PropertyType corresponding to a PropertyValue
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param value The property value to inspect
     * @return PropertyType The type of the stored value
     */
    inline PropertyType getPropertyTypeFromValue(const PropertyValue& value) {
        return std::visit([](auto&& arg) -> PropertyType {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) return PropertyType::Unknown;
            else if constexpr (std::is_same_v<T, bool>) return PropertyType::Bool;
            else if constexpr (std::is_same_v<T, int>) return PropertyType::Int;
            else if constexpr (std::is_same_v<T, float>) return PropertyType::Float;
            else if constexpr (std::is_same_v<T, std::string>) return PropertyType::String;
            else if constexpr (std::is_same_v<T, ImVec4>) return PropertyType::Color;
            else if constexpr (std::is_same_v<T, ImVec2>) return PropertyType::Vector2;
            else if constexpr (std::is_same_v<T, EnumValue>) return PropertyType::Enum;
            else return PropertyType::Unknown;
        }, value);
    }

    /**
     * @brief Check if a PropertyValue holds a specific type
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @tparam T The type to check for
     * @param value The property value to inspect
     * @return true if the value holds type T
     * @return false otherwise
     */
    template<typename T>
    inline bool holdsType(const PropertyValue& value) {
        return std::holds_alternative<T>(value);
    }

    /**
     * @brief Safely get a value from PropertyValue with a default
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @tparam T The type to extract
     * @param value The property value
     * @param defaultValue The default value if type doesn't match
     * @return T The extracted value or default
     */
    template<typename T>
    inline T getValueOr(const PropertyValue& value, const T& defaultValue) {
        if (auto* ptr = std::get_if<T>(&value)) {
            return *ptr;
        }
        return defaultValue;
    }
}

#endif //ADS_PROPERTY_VALUE_H