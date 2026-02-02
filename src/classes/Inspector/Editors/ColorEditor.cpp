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
 * @file ColorEditor.cpp
 * @brief Implementation of the ColorEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "ColorEditor.h"
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
    std::vector<PropertyType> ColorEditor::getSupportedTypes() const {
        return { PropertyType::Color };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a color picker for editing RGBA color properties.
     * Uses ImGui::ColorEdit4 with a color preview button.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult ColorEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        ImVec4 color = getValueOr<ImVec4>(currentValue, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImVec4 originalColor = color;

        ImGui::PushID(descriptor.getId().c_str());

        // Two-column layout: label on left, widget on right
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 120.0f);

        // Label column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        ImGui::NextColumn();

        // Widget column
        if (readOnly) {
            ImGui::BeginDisabled();
        }

        // Use ColorEdit4 for RGBA editing with a nice color picker
        ImGuiColorEditFlags flags =
            ImGuiColorEditFlags_AlphaBar |
            ImGuiColorEditFlags_AlphaPreview |
            ImGuiColorEditFlags_PickerHueWheel;

        ImGui::SetNextItemWidth(-1);
        ImGui::ColorEdit4(
            "##value",
            reinterpret_cast<float*>(&color),
            flags
        );

        if (readOnly) {
            ImGui::EndDisabled();
        }

        ImGui::Columns(1);
        ImGui::PopID();

        // Check if color changed (compare all components)
        if (color.x != originalColor.x ||
            color.y != originalColor.y ||
            color.z != originalColor.z ||
            color.w != originalColor.w) {
            return EditResult::modified(color);
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
    std::string ColorEditor::getEditorId() const {
        return "ColorEditor";
    }
}
