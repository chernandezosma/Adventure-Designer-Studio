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

#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include <vector>
#include "PropertyType.h"
#include "PropertyConstraints.h"
#include "PropertyValue.h"

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
        std::string m_subcategory;          // Optional one-level nested group inside the category

        /// i18n key for the "nothing to choose from" notice a PropertyType::Select
        /// / Enum editor shows when its option list resolves empty. Defaults to
        /// the generic "INSPECTOR.NO_OPTIONS"; state-backed fields override it
        /// with "INSPECTOR.NO_STATES" so the message names what is missing.
        std::string m_emptyOptionsTextKey = "INSPECTOR.NO_OPTIONS";
        PropertyType m_type;                // Property value type
        PropertyConstraints m_constraints;  // Value constraints
        bool m_readOnly;                    // Whether the property can be edited
        bool m_multiSelect = false;         // For PropertyType::Select: single- vs multi-select

        /// For a single-select PropertyType::Select field whose options are
        /// drawn from a live, author-extensible catalog (e.g. Scene's
        /// "state" property, whose options are the project's state chains):
        /// shows a "+" button next to the dropdown to create a new option
        /// inline instead of requiring the author to leave the inspector.
        bool m_allowCreateNew = false;

        /// For PropertyType::Select multi-select fields backed by a bitmap
        /// (e.g. Scene's affordances/flags): maps an option index to the
        /// bit position it represents, for options whose meaning is
        /// author-chosen ("user-defined") rather than fixed. Lets
        /// SelectEditor detect "user just checked a user-defined option"
        /// without string-matching translated placeholder text.
        std::vector<std::pair<int, uint8_t>> m_userDefinedOptionBits;

        /// Optional visibility condition callback
        std::function<bool(const IInspectable*)> m_visibilityCondition;

        /// Optional dynamic option-list provider, for PropertyType::Select
        /// fields whose choices depend on live project state (e.g. "all
        /// scenes", "all items") rather than a fixed list. Falls back to
        /// PropertyConstraints::enumValues when unset.
        std::function<std::vector<std::string>()> m_optionsProvider;

        /// Preset-key provider for a PropertyType::AffordanceList field,
        /// index-paired with m_optionsProvider — see setPresetKeys().
        std::function<std::vector<std::string>()> m_presetKeysProvider;

        /// Provider for a PropertyType::AffordanceList field's two dropdown
        /// vocabularies (affordance-name presets and this entity's own
        /// trigger names) — see Inspector::AffordanceOptions.
        std::function<AffordanceOptions()> m_affordanceOptionsProvider;

        /// For a multi-select PropertyType::Select field representing one
        /// of an entity's fixed global trigger keys (Scene/Item/Character's
        /// "Triggers" category rows): shows a "..." button next to the
        /// checklist as the future hook for a dedicated trigger-effect
        /// editor (not built yet — see EditResult::requestTriggerEditor()).
        bool m_showTriggerEditorButton = false;

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
         * @brief Set the description (tooltip text) — lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param desc Description text
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setDescription(const std::string& desc);

        /**
         * @brief Set the description (tooltip text) — rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param desc Description text
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setDescription(std::string&& desc);

        /**
         * @brief Set the category for grouping — lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param category Category name
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setCategory(const std::string& category);

        /**
         * @brief Set the category for grouping — rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param category Category name
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setCategory(std::string&& category);

        /**
         * @brief Set value constraints — lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param constraints Property constraints
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setConstraints(const PropertyConstraints& constraints);

        /**
         * @brief Set value constraints — rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param constraints Property constraints
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setConstraints(PropertyConstraints&& constraints);

        /**
         * @brief Mark property as read-only
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
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

        /**
         * @brief Mark a PropertyType::Select property as multi-select
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param multiSelect True for a checklist-style multi-select, false
         *                    (default) for a single-select dropdown
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setMultiSelect(bool multiSelect = true);

        /**
         * @brief Show a "+" button next to a single-select dropdown to create a new option inline
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param allowCreateNew True to show the button (default: true)
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setAllowCreateNew(bool allowCreateNew = true);

        /**
         * @brief Set a dynamic option-list provider for PropertyType::Select
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Invoked by SelectEditor at render time to resolve the current
         * option list (e.g. "every scene name in the project"). Falls back
         * to PropertyConstraints::enumValues when unset.
         *
         * @param provider Callable returning the current option list
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setOptionsProvider(
            std::function<std::vector<std::string>()> provider
        );

        /**
         * @brief Set the preset-key provider for a PropertyType::AffordanceList field
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Index-paired with setOptionsProvider(): element i of this
         * provider's result is the untranslated key behind element i of
         * the options provider's translated label. Lets AffordanceListEditor
         * remember which preset an added entry came from.
         *
         * @param provider Callable returning the current preset-key list
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setPresetKeys(
            std::function<std::vector<std::string>()> provider
        );

        /**
         * @brief Mark which option indices are "user-defined" bitmap bits
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param indexToBitPosition One (option index, bit position) pair
         *                           per user-defined option
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setUserDefinedOptionBits(
            std::vector<std::pair<int, uint8_t>> indexToBitPosition
        );

        /**
         * @brief Set a one-level nested group name inside the category — lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Rows sharing a category and a non-empty subcategory are rendered
         * under a collapsible node inside that category (e.g. Item's Effects
         * category split into "Damage" and "Heal"). Rows with an empty
         * subcategory render directly under the category as before.
         *
         * @param subcategory Nested group name (already translated)
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setSubcategory(const std::string& subcategory);

        /**
         * @brief Set a one-level nested group name inside the category — rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param subcategory Nested group name (already translated)
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setSubcategory(std::string&& subcategory);

        /**
         * @brief Set the i18n key for the empty-option-list notice — lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param key i18n key resolved by the Select/Enum editor when it has
         *            no options to show
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setEmptyOptionsText(const std::string& key);

        /**
         * @brief Set the i18n key for the empty-option-list notice — rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param key i18n key resolved by the Select/Enum editor when it has
         *            no options to show
         * @return PropertyDescriptor& Reference for chaining
         */
        PropertyDescriptor& setEmptyOptionsText(std::string&& key);

        // Getters

        /**
         * @brief Get the unique property ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Property identifier
         */
        [[nodiscard]] const std::string& getId() const;

        /**
         * @brief Get the display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Human-readable name
         */
        [[nodiscard]] const std::string& getDisplayName() const;

        /**
         * @brief Get the description
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Description/tooltip text
         */
        [[nodiscard]] const std::string& getDescription() const;

        /**
         * @brief Get the category
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Category name
         */
        [[nodiscard]] const std::string& getCategory() const;

        /**
         * @brief Get the nested subcategory name, if any
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& Subcategory name; empty when the row
         *                             renders directly under its category
         */
        [[nodiscard]] const std::string& getSubcategory() const;

        /**
         * @brief Get the i18n key for the empty-option-list notice
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& i18n key (defaults to "INSPECTOR.NO_OPTIONS")
         */
        [[nodiscard]] const std::string& getEmptyOptionsTextKey() const;

        /**
         * @brief Get the property type
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return PropertyType The property value type
         */
        [[nodiscard]] PropertyType getType() const;

        /**
         * @brief Get the value constraints
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const PropertyConstraints& Property constraints
         */
        [[nodiscard]] const PropertyConstraints& getConstraints() const;

        /**
         * @brief Check if property is read-only
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if read-only, false if editable
         */
        [[nodiscard]] bool isReadOnly() const;

        /**
         * @brief Check if a PropertyType::Select property is multi-select
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return bool True for checklist-style multi-select
         */
        [[nodiscard]] bool isMultiSelect() const;

        /**
         * @brief Check whether a "+" create-new button should be shown next to a single-select dropdown
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return bool True if the button should be shown
         */
        [[nodiscard]] bool isAllowCreateNew() const;

        /**
         * @brief Get the dynamic option-list provider, if set
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::function<std::vector<std::string>()>& May be empty/unset
         */
        [[nodiscard]] const std::function<std::vector<std::string>()>& getOptionsProvider() const;

        /**
         * @brief Get the preset-key provider, if set
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return const std::function<std::vector<std::string>()>& May be empty/unset
         */
        [[nodiscard]] const std::function<std::vector<std::string>()>& getPresetKeysProvider() const;

        /**
         * @brief Check whether a Select option index is a user-defined bitmap bit
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param optionIndex Index into the option list
         * @return bool True if that option was marked via setUserDefinedOptionBits()
         */
        [[nodiscard]] bool isUserDefinedOption(int optionIndex) const;

        /**
         * @brief Get the bit position a user-defined option index represents
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Only meaningful when isUserDefinedOption(optionIndex) is true.
         *
         * @param optionIndex Index into the option list
         * @return uint8_t The bit position, or 0 if optionIndex isn't user-defined
         */
        [[nodiscard]] uint8_t getUserDefinedOptionBit(int optionIndex) const;

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