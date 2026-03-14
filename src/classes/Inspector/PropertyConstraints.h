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

#ifndef ADS_PROPERTY_CONSTRAINTS_H
#define ADS_PROPERTY_CONSTRAINTS_H

#include <optional>
#include <string>
#include <vector>

namespace ADS::Inspector {
    /**
     * @brief Constraints for property values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Defines validation and display constraints for property values.
     * Different constraint types apply to different property types:
     * - Numeric constraints (min, max, step) for Int and Float
     * - String constraints (maxLength) for String
     * - Enum constraints (enumValues) for Enum
     */
    struct PropertyConstraints {
        // Numeric constraints (for Int and Float types)
        std::optional<float> minValue;      ///< Minimum allowed value
        std::optional<float> maxValue;      ///< Maximum allowed value
        std::optional<float> step;          ///< Increment step for sliders/drag controls

        // String constraints
        std::optional<size_t> maxLength;    ///< Maximum string length

        // Enum constraints
        std::vector<std::string> enumValues; ///< Available enum options

        /**
         * @brief Default constructor
         */
        PropertyConstraints() = default;

        /**
         * @brief Create constraints for numeric types
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param min Minimum allowed value
         * @param max Maximum allowed value
         * @param stepValue Increment step (default: 1.0f)
         * @return PropertyConstraints Configured constraints
         */
        static PropertyConstraints numeric(float min, float max, float stepValue = 1.0f);

        /**
         * @brief Create constraints for string types
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param maxLen Maximum string length
         * @return PropertyConstraints Configured constraints
         */
        static PropertyConstraints string(size_t maxLen);

        /**
         * @brief Create constraints for enum types
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param values Available enum options
         * @return PropertyConstraints Configured constraints
         */
        static PropertyConstraints enumeration(std::vector<std::string> values);

        /**
         * @brief Check if numeric constraints are defined
         *
         * @return true if min or max values are set
         */
        bool hasNumericConstraints() const;

        /**
         * @brief Check if string constraints are defined
         *
         * @return true if maxLength is set
         */
        bool hasStringConstraints() const;

        /**
         * @brief Check if enum constraints are defined
         *
         * @return true if enumValues is not empty
         */
        bool hasEnumConstraints() const;
    };
}

#endif //ADS_PROPERTY_CONSTRAINTS_H