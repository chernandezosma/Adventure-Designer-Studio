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
     * @brief Mark a PropertyType::Select property as multi-select
     *
     * @param multiSelect True for a checklist-style multi-select
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setMultiSelect(bool multiSelect)
    {
        m_multiSelect = multiSelect;

        return *this;
    }

    /**
     * @brief Show a "+" button next to a single-select dropdown to create a new option inline
     *
     * @param allowCreateNew True to show the button
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setAllowCreateNew(bool allowCreateNew)
    {
        m_allowCreateNew = allowCreateNew;

        return *this;
    }

    /**
     * @brief Set a dynamic option-list provider for PropertyType::Select
     *
     * @param provider Callable returning the current option list
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setOptionsProvider(
            std::function<std::vector<std::string>()> provider
            )
    {
        m_optionsProvider = std::move(provider);

        return *this;
    }

    /**
     * @brief Set the preset-key provider for a PropertyType::AffordanceList field
     *
     * @param provider Callable returning the current preset-key list
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setPresetKeys(
            std::function<std::vector<std::string>()> provider
            )
    {
        m_presetKeysProvider = std::move(provider);

        return *this;
    }

    /**
     * @brief Mark which option indices are "user-defined" bitmap bits
     *
     * @param indexToBitPosition One (option index, bit position) pair per user-defined option
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setUserDefinedOptionBits(
            std::vector<std::pair<int, uint8_t>> indexToBitPosition
            )
    {
        m_userDefinedOptionBits = std::move(indexToBitPosition);

        return *this;
    }

    /**
     * @brief Set a one-level nested group name inside the category - lvalue version
     *
     * @param subcategory Nested group name (already translated)
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setSubcategory(const std::string& subcategory)
    {
        m_subcategory = subcategory;

        return *this;
    }

    /**
     * @brief Set a one-level nested group name inside the category - rvalue version
     *
     * @param subcategory Nested group name (already translated)
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setSubcategory(std::string&& subcategory)
    {
        m_subcategory = std::move(subcategory);

        return *this;
    }

    /**
     * @brief Set the i18n key for the empty-option-list notice - lvalue version
     *
     * @param key i18n key resolved by the Select/Enum editor when it has no options
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setEmptyOptionsText(const std::string& key)
    {
        m_emptyOptionsTextKey = key;

        return *this;
    }

    /**
     * @brief Set the i18n key for the empty-option-list notice - rvalue version
     *
     * @param key i18n key resolved by the Select/Enum editor when it has no options
     * @return PropertyDescriptor& Reference for chaining
     */
    PropertyDescriptor &PropertyDescriptor::setEmptyOptionsText(std::string&& key)
    {
        m_emptyOptionsTextKey = std::move(key);

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
     * @brief Get the nested subcategory name, if any
     * @return const std::string& Subcategory name; empty when unset
     */
    const std::string &PropertyDescriptor::getSubcategory() const
    {
        return m_subcategory;
    }

    /**
     * @brief Get the i18n key for the empty-option-list notice
     * @return const std::string& i18n key (defaults to "INSPECTOR.NO_OPTIONS")
     */
    const std::string &PropertyDescriptor::getEmptyOptionsTextKey() const
    {
        return m_emptyOptionsTextKey;
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
     * @brief Check if a PropertyType::Select property is multi-select
     * @return bool True for checklist-style multi-select
     */
    bool PropertyDescriptor::isMultiSelect() const
    {
        return m_multiSelect;
    }

    /**
     * @brief Check whether a "+" create-new button should be shown next to a single-select dropdown
     * @return bool True if the button should be shown
     */
    bool PropertyDescriptor::isAllowCreateNew() const
    {
        return m_allowCreateNew;
    }

    /**
     * @brief Get the dynamic option-list provider, if set
     * @return const std::function<std::vector<std::string>()>& May be empty/unset
     */
    const std::function<std::vector<std::string>()> &PropertyDescriptor::getOptionsProvider() const
    {
        return m_optionsProvider;
    }

    /**
     * @brief Get the preset-key provider, if set
     * @return const std::function<std::vector<std::string>()>& May be empty/unset
     */
    const std::function<std::vector<std::string>()> &PropertyDescriptor::getPresetKeysProvider() const
    {
        return m_presetKeysProvider;
    }

    /**
     * @brief Check whether a Select option index is a user-defined bitmap bit
     * @param optionIndex Index into the option list
     * @return bool True if that option was marked via setUserDefinedOptionBits()
     */
    bool PropertyDescriptor::isUserDefinedOption(int optionIndex) const
    {
        for (const auto& [index, bitPosition] : m_userDefinedOptionBits) {
            if (index == optionIndex) return true;
        }
        return false;
    }

    /**
     * @brief Get the bit position a user-defined option index represents
     * @param optionIndex Index into the option list
     * @return uint8_t The bit position, or 0 if optionIndex isn't user-defined
     */
    uint8_t PropertyDescriptor::getUserDefinedOptionBit(int optionIndex) const
    {
        for (const auto& [index, bitPosition] : m_userDefinedOptionBits) {
            if (index == optionIndex) return bitPosition;
        }
        return 0;
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
