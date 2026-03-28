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
 * @file BoolEditor.cpp
 * @brief Implementation of the BoolEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "BoolEditor.h"
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
    std::vector<PropertyType> BoolEditor::getSupportedTypes() const {
        return { PropertyType::Bool };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a checkbox for editing boolean properties.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult BoolEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        bool value = getValueOr<bool>(currentValue, false);
        bool originalValue = value;

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
        if (readOnly) {
            ImGui::BeginDisabled();
        }

        ImGui::Checkbox("##value", &value);

        if (readOnly) {
            ImGui::EndDisabled();
        }

        // Show tooltip on widget hover
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (value != originalValue) {
            return EditResult::modified(value);
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
    std::string BoolEditor::getEditorId() const {
        return "BoolEditor";
    }
}
