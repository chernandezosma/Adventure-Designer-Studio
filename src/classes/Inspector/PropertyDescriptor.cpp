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
 * @file PropertyDescriptor.cpp
 * @brief Implementation of the PropertyDescriptor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "PropertyDescriptor.h"

namespace ADS::Inspector {

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
    PropertyDescriptor::PropertyDescriptor(
            std::string id,
            std::string displayName,
            PropertyType type
            ) :
        m_id(std::move(id)),
        m_displayName(std::move(displayName)),
        m_type(type),
        m_readOnly(false)
    {
    }

    /**
     * @brief Set the description (tooltip text) - lvalue version
     *
     * @param desc Description text
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setDescription(const std::string& desc)
    {
        m_description = desc;

        return *this;
    }

    /**
     * @brief Set the description (tooltip text) - rvalue version
     *
     * @param desc Description text
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setDescription(std::string&& desc)
    {
        m_description = std::move(desc);

        return *this;
    }

    /**
     * @brief Set the category for grouping - lvalue version
     *
     * @param category Category name
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setCategory(const std::string& category)
    {
        m_category = category;

        return *this;
    }

    /**
     * @brief Set the category for grouping - rvalue version
     *
     * @param category Category name
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setCategory(std::string&& category)
    {
        m_category = std::move(category);

        return *this;
    }

    /**
     * @brief Set value constraints - lvalue version
     *
     * @param constraints Property constraints
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setConstraints(const PropertyConstraints& constraints)
    {
        m_constraints = constraints;

        return *this;
    }

    /**
     * @brief Set value constraints - rvalue version
     *
     * @param constraints Property constraints
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setConstraints(PropertyConstraints&& constraints)
    {
        m_constraints = std::move(constraints);

        return *this;
    }

    /**
     * @brief Mark property as read-only
     *
     * @param readOnly True to make read-only (default: true)
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;

        return *this;
    }

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
    PropertyDescriptor &PropertyDescriptor::setVisibilityCondition(
            const std::function<bool(const IInspectable *)>& condition
            )
    {
        m_visibilityCondition = condition;

        return *this;
    }

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
    PropertyDescriptor &PropertyDescriptor::setVisibilityCondition(
            std::function<bool(const IInspectable *)>&& condition
            )
    {
        m_visibilityCondition = std::move(condition);

        return *this;
    }

    /**
     * @brief Get the unique property ID
     * @return const std::string& Property identifier
     */
    const std::string &PropertyDescriptor::getId() const
    {
        return m_id;
    }

    /**
     * @brief Get the display name
     * @return const std::string& Human-readable name
     */
    const std::string &PropertyDescriptor::getDisplayName() const
    {
        return m_displayName;
    }

    /**
     * @brief Get the description
     * @return const std::string& Description/tooltip text
     */
    const std::string &PropertyDescriptor::getDescription() const
    {
        return m_description;
    }

    /**
     * @brief Get the category
     * @return const std::string& Category name
     */
    const std::string &PropertyDescriptor::getCategory() const
    {
        return m_category;
    }

    /**
     * @brief Get the property type
     * @return PropertyType The property type
     */
    PropertyType PropertyDescriptor::getType() const
    {
        return m_type;
    }

    /**
     * @brief Get the constraints
     * @return const PropertyConstraints& Property constraints
     */
    const PropertyConstraints &PropertyDescriptor::getConstraints() const
    {
        return m_constraints;
    }

    /**
     * @brief Check if property is read-only
     * @return true if read-only, false if editable
     */
    bool PropertyDescriptor::isReadOnly() const
    {
        return m_readOnly;
    }

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
    bool PropertyDescriptor::isVisible(const IInspectable *target) const
    {
        if (!m_visibilityCondition) {
            return true;
        }
        return m_visibilityCondition(target);
    }
}
