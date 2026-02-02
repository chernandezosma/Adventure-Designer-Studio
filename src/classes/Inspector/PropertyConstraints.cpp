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