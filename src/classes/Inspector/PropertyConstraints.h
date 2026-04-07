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
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if min or max values are set
         */
        bool hasNumericConstraints() const;

        /**
         * @brief Check if string constraints are defined
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if maxLength is set
         */
        bool hasStringConstraints() const;

        /**
         * @brief Check if enum constraints are defined
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if enumValues is not empty
         */
        bool hasEnumConstraints() const;
    };
}

#endif //ADS_PROPERTY_CONSTRAINTS_H