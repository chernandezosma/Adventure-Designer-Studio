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

#ifndef ADS_PROPERTY_DESCRIPTOR_H
#define ADS_PROPERTY_DESCRIPTOR_H

#include <string>
#include <functional>
#include "PropertyType.h"
#include "PropertyConstraints.h"

namespace ADS::Inspector {
    // Forward declaration
    class IInspectable;

    /**
     * @brief Metadata descriptor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Contains all metadata about a property including its identifier,
     * display information, type, constraints, and visibility conditions.
     * Uses a fluent API for easy configuration.
     */
    class PropertyDescriptor {
    private:
        std::string m_id;                   // Unique property identifier
        std::string m_displayName;          // Human-readable name for UI
        std::string m_description;          // Tooltip/help text
        std::string m_category;             // Category grouping (e.g., "General", "Appearance")
        PropertyType m_type;                // Property value type
        PropertyConstraints m_constraints;  // Value constraints
        bool m_readOnly;                    // Whether the property can be edited

        /// Optional visibility condition callback
        std::function<bool(const IInspectable*)> m_visibilityCondition;

    public:
        /**
         * @brief Construct a new PropertyDescriptor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param id Unique property identifier
         * @param displayName Human-readable name
         * @param type Property value type
         */
        PropertyDescriptor(
            std::string id,
            std::string displayName,
            PropertyType type
        );

        /**
         * @brief Set the description (tooltip text) - lvalue version
         *
         * @param desc Description text
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setDescription(const std::string& desc);

        /**
         * @brief Set the description (tooltip text) - rvalue version
         *
         * @param desc Description text
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setDescription(std::string&& desc);

        /**
         * @brief Set the category for grouping - lvalue version
         *
         * @param category Category name
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setCategory(const std::string& category);

        /**
         * @brief Set the category for grouping - rvalue version
         *
         * @param category Category name
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setCategory(std::string&& category);

        /**
         * @brief Set value constraints - lvalue version
         *
         * @param constraints Property constraints
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setConstraints(const PropertyConstraints& constraints);

        /**
         * @brief Set value constraints - rvalue version
         *
         * @param constraints Property constraints
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setConstraints(PropertyConstraints&& constraints);

        /**
         * @brief Mark property as read-only
         *
         * @param readOnly True to make read-only (default: true)
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setReadOnly(bool readOnly = true);

        /**
         * @brief Set visibility condition callback - lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * The callback is invoked to determine if the property should
         * be displayed based on the current state of the inspectable object.
         *
         * @param condition Function that returns true if property should be visible
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setVisibilityCondition(
            const std::function<bool(const IInspectable*)>& condition
        );

        /**
         * @brief Set visibility condition callback - rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * The callback is invoked to determine if the property should
         * be displayed based on the current state of the inspectable object.
         *
         * @param condition Function that returns true if property should be visible
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setVisibilityCondition(
            std::function<bool(const IInspectable*)>&& condition
        );

        // Getters

        /**
         * @brief Get the unique property ID
         * @return const std::string& Property identifier
         */
        [[nodiscard]] const std::string& getId() const;

        /**
         * @brief Get the display name
         * @return const std::string& Human-readable name
         */
        [[nodiscard]] const std::string& getDisplayName() const;

        /**
         * @brief Get the description
         * @return const std::string& Description/tooltip text
         */
        [[nodiscard]] const std::string& getDescription() const;

        /**
         * @brief Get the category
         * @return const std::string& Category name
         */
        [[nodiscard]] const std::string& getCategory() const;

        /**
         * @brief Get the property type
         * @return PropertyType The property type
         */
        [[nodiscard]] PropertyType getType() const;

        /**
         * @brief Get the constraints
         * @return const PropertyConstraints& Property constraints
         */
        [[nodiscard]] const PropertyConstraints& getConstraints() const;

        /**
         * @brief Check if property is read-only
         * @return true if read-only, false if editable
         */
         [[nodiscard]] bool isReadOnly() const;

        /**
         * @brief Check if property should be visible
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Evaluates the visibility condition if one is set.
         * Returns true if no condition is set.
         *
         * @param target The inspectable object being inspected
         * @return true if property should be displayed
         */
        bool isVisible(const IInspectable* target) const;
    };
}

#endif //ADS_PROPERTY_DESCRIPTOR_H