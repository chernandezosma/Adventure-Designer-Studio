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

#ifndef ADS_PROPERTY_VALUE_H
#define ADS_PROPERTY_VALUE_H

#include <map>
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
        int selectedIndex = 0; ///< Index of the currently selected option
        std::vector<std::string> options; ///< The full list of choices

        EnumValue() = default;
        EnumValue(int index, std::vector<std::string> opts)
            : selectedIndex(index), options(std::move(opts)) {}
    };

    /**
     * @brief Type alias for single- or multi-select value storage
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Stores the selected option index/indices and the option list backing
     * a PropertyType::Select property. Single-select fields hold at most
     * one index in selectedIndices (empty means "no selection"); multi-
     * select fields may hold any number.
     */
    struct SelectValue {
        std::vector<int> selectedIndices; ///< Selected option index/indices (empty = none)
        std::vector<std::string> options; ///< The full list of choices

        SelectValue() = default;
        SelectValue(std::vector<int> indices, std::vector<std::string> opts)
            : selectedIndices(std::move(indices)), options(std::move(opts)) {}
    };

    /**
     * @brief One row of a PropertyType::AffordanceList value
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Mirrors Data::Affordance's shape without depending on Data/ — the
     * Inspector layer stays decoupled from Data/, converted 1:1 at the
     * entity boundary (see e.g. Entities::Item::getPropertyValue()).
     */
    struct AffordanceEntry {
        std::string name; ///< The affordance's display name (e.g. "Open")
        std::vector<std::string> triggers; ///< Trigger names attached to this affordance
        std::string presetKey; ///< Translation key this was picked from, or empty if custom-typed

        AffordanceEntry() = default;
        AffordanceEntry(std::string n, std::vector<std::string> t, std::string key = {})
            : name(std::move(n)), triggers(std::move(t)), presetKey(std::move(key)) {}
    };

    /**
     * @brief Type alias for a PropertyType::AffordanceList value
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     */
    using AffordanceListValue = std::vector<AffordanceEntry>;

    /**
     * @brief Dropdown vocabularies backing a PropertyType::AffordanceList editor
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Returned by PropertyDescriptor::getAffordanceOptionsProvider(). Bundles
     * the two independent dropdowns AffordanceListEditor renders per row:
     * - Affordance name: nameLabels/nameKeys, index-paired (translated
     *   preset label -> the translation key it came from, e.g.
     *   "ITEM.AFF_OPENABLE"). Empty when the entity has no known preset
     *   list (e.g. Character today) — the editor falls back to free text.
     * - Trigger: triggerLabels/triggerValues, index-paired (translated
     *   trigger label -> the entity's own canonical global trigger name,
     *   e.g. "on_examine" — the same vocabulary shown under the "Triggers"
     *   category, see Scene/Item/Character's own triggerKeys()).
     */
    struct AffordanceOptions {
        std::vector<std::string> nameLabels;
        std::vector<std::string> nameKeys;
        std::vector<std::string> triggerLabels;
        std::vector<std::string> triggerValues;
    };

    /**
     * @brief Per-language text storage for a translatable String property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Keyed by BCP-47/POSIX language code (e.g. "es_ES"), matching
     * ADS::Constants::Languages' code format and LexEngine::LanguageCode.
     * Deliberately not the same type as Data::LocalizedText — Inspector/
     * stays decoupled from Data/, converted 1:1 at the entity boundary
     * (see e.g. Entities::Scene::getPropertyValue()).
     */
    using LocalizedText = std::map<std::string, std::string>;

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
        EnumValue,          ///< Enumeration value
        SelectValue,        ///< Single- or multi-select value
        LocalizedText,      ///< Per-language text (translatable String fields)
        AffordanceListValue ///< Editable list of named affordances + their triggers
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
            else if constexpr (std::is_same_v<T, SelectValue>) return PropertyType::Select;
            else if constexpr (std::is_same_v<T, LocalizedText>) return PropertyType::String;
            else if constexpr (std::is_same_v<T, AffordanceListValue>) return PropertyType::AffordanceList;
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