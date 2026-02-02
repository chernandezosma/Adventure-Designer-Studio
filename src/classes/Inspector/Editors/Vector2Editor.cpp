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
 * @file Vector2Editor.cpp
 * @brief Implementation of the Vector2Editor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "Vector2Editor.h"
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
    std::vector<PropertyType> Vector2Editor::getSupportedTypes() const {
        return { PropertyType::Vector2 };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders two drag float controls for editing X and Y components
     * of a 2D vector (ImVec2).
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult Vector2Editor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        ImVec2 vec = getValueOr<ImVec2>(currentValue, ImVec2(0.0f, 0.0f));
        ImVec2 originalVec = vec;

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

        // Get speed from constraints if available
        float speed = 0.1f;
        const auto& constraints = descriptor.getConstraints();
        if (constraints.step.has_value()) {
            speed = constraints.step.value();
        }

        // Use DragFloat2 for compact two-component editing
        float values[2] = { vec.x, vec.y };
        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat2(
            "##value",
            values,
            speed,
            0.0f,
            0.0f,
            "%.3f"
        );
        vec.x = values[0];
        vec.y = values[1];

        // Apply constraints if specified
        if (constraints.minValue.has_value()) {
            float minVal = constraints.minValue.value();
            if (vec.x < minVal) vec.x = minVal;
            if (vec.y < minVal) vec.y = minVal;
        }
        if (constraints.maxValue.has_value()) {
            float maxVal = constraints.maxValue.value();
            if (vec.x > maxVal) vec.x = maxVal;
            if (vec.y > maxVal) vec.y = maxVal;
        }

        if (readOnly) {
            ImGui::EndDisabled();
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (vec.x != originalVec.x || vec.y != originalVec.y) {
            return EditResult::modified(vec);
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
    std::string Vector2Editor::getEditorId() const {
        return "Vector2Editor";
    }
}
