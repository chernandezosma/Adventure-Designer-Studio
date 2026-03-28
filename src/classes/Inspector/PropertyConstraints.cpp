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

/**
 * @file PropertyConstraints.cpp
 * @brief Implementation of the PropertyConstraints class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "PropertyConstraints.h"

namespace ADS::Inspector {
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
    PropertyConstraints PropertyConstraints::numeric(float min, float max, float stepValue) {
        PropertyConstraints constraints;
        constraints.minValue = min;
        constraints.maxValue = max;
        constraints.step = stepValue;
        return constraints;
    }

    /**
     * @brief Create constraints for string types
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param maxLen Maximum string length
     * @return PropertyConstraints Configured constraints
     */
    PropertyConstraints PropertyConstraints::string(size_t maxLen) {
        PropertyConstraints constraints;
        constraints.maxLength = maxLen;
        return constraints;
    }

    /**
     * @brief Create constraints for enum types
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param values Available enum options
     * @return PropertyConstraints Configured constraints
     */
    PropertyConstraints PropertyConstraints::enumeration(std::vector<std::string> values) {
        PropertyConstraints constraints;
        constraints.enumValues = std::move(values);
        return constraints;
    }

    /**
     * @brief Check if numeric constraints are defined
     *
     * @return true if min or max values are set
     */
    bool PropertyConstraints::hasNumericConstraints() const {
        return minValue.has_value() || maxValue.has_value();
    }

    /**
     * @brief Check if string constraints are defined
     *
     * @return true if maxLength is set
     */
    bool PropertyConstraints::hasStringConstraints() const {
        return maxLength.has_value();
    }

    /**
     * @brief Check if enum constraints are defined
     *
     * @return true if enumValues is not empty
     */
    bool PropertyConstraints::hasEnumConstraints() const {
        return !enumValues.empty();
    }
}