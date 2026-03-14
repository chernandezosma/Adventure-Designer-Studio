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
 * @file EnumEditor.cpp
 * @brief Implementation of the EnumEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "EnumEditor.h"
#include "imgui.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Get the property types this editor can handle
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @return std::vector<PropertyType> List of supported types
     */
    std::vector<PropertyType> EnumEditor::getSupportedTypes() const {
        return { PropertyType::Enum };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a combo box (dropdown) for selecting from predefined
     * enum values. The available options are stored in the EnumValue
     * or taken from the property constraints.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult EnumEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        // Get current enum value
        EnumValue enumVal = getValueOr<EnumValue>(currentValue, EnumValue());

        // Get options from value or constraints
        std::vector<std::string> options = enumVal.options;
        if (options.empty() && descriptor.getConstraints().hasEnumConstraints()) {
            options = descriptor.getConstraints().enumValues;
        }

        ImGui::PushID(descriptor.getId().c_str());

        // Two-column layout: label on left, widget on right
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 120.0f);

        // Label column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        // Show tooltip on label hover
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }
        ImGui::NextColumn();

        // Widget column
        if (options.empty()) {
            ImGui::TextDisabled("No options available");
            ImGui::Columns(1);
            ImGui::PopID();
            return EditResult::unchanged();
        }

        int selectedIndex = enumVal.selectedIndex;
        int originalIndex = selectedIndex;

        // Clamp index to valid range
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(options.size())) {
            selectedIndex = 0;
        }

        if (readOnly) {
            ImGui::BeginDisabled();
        }

        // Build items string for combo (null-separated)
        std::string itemsStr;
        for (const auto& opt : options) {
            itemsStr += opt;
            itemsStr += '\0';
        }
        itemsStr += '\0';

        ImGui::SetNextItemWidth(-1);
        ImGui::Combo(
            "##value",
            &selectedIndex,
            itemsStr.c_str()
        );

        if (readOnly) {
            ImGui::EndDisabled();
        }

        // Show tooltip on widget hover
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (selectedIndex != originalIndex) {
            EnumValue newValue(selectedIndex, options);
            return EditResult::modified(newValue);
        }

        return EditResult::unchanged();
    }

    /**
     * @brief Get a unique identifier for this editor type
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @return std::string Unique editor identifier
     */
    std::string EnumEditor::getEditorId() const {
        return "EnumEditor";
    }
}
