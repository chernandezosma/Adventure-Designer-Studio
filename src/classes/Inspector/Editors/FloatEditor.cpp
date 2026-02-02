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
 * @file FloatEditor.cpp
 * @brief Implementation of the FloatEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "FloatEditor.h"
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
    std::vector<PropertyType> FloatEditor::getSupportedTypes() const {
        return { PropertyType::Float };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a slider or drag control for editing float properties.
     * Uses SliderFloat when min/max constraints are specified, otherwise
     * uses DragFloat for unconstrained values.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult FloatEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        float value = getValueOr<float>(currentValue, 0.0f);
        float originalValue = value;

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

        ImGui::SetNextItemWidth(-1);
        const auto& constraints = descriptor.getConstraints();

        if (constraints.hasNumericConstraints() &&
            constraints.minValue.has_value() &&
            constraints.maxValue.has_value()) {
            // Use slider when we have both min and max
            ImGui::SliderFloat(
                "##value",
                &value,
                constraints.minValue.value(),
                constraints.maxValue.value(),
                "%.3f"
            );
        } else {
            // Use drag for unconstrained or partially constrained values
            float speed = 0.1f;
            if (constraints.step.has_value()) {
                speed = constraints.step.value();
            }
            ImGui::DragFloat(
                "##value",
                &value,
                speed,
                0.0f,
                0.0f,
                "%.3f"
            );

            // Apply constraints if only one bound is specified
            if (constraints.minValue.has_value() && value < constraints.minValue.value()) {
                value = constraints.minValue.value();
            }
            if (constraints.maxValue.has_value() && value > constraints.maxValue.value()) {
                value = constraints.maxValue.value();
            }
        }

        if (readOnly) {
            ImGui::EndDisabled();
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
    std::string FloatEditor::getEditorId() const {
        return "FloatEditor";
    }
}
