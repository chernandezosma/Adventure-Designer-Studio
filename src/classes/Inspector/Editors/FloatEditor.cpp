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
 * @file FloatEditor.cpp
 * @brief Implementation of the FloatEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "FloatEditor.h"
#include "EditorLayout.h"
#include "imgui.h"

#include <algorithm>
#include <string>

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
        beginPropertyColumns(descriptor);

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

        const auto& constraints = descriptor.getConstraints();
        const bool hasRange = constraints.hasNumericConstraints() &&
                              constraints.minValue.has_value() &&
                              constraints.maxValue.has_value();

        bool widgetHovered = false;

        if (hasRange) {
            const float minVal = constraints.minValue.value();
            const float maxVal = constraints.maxValue.value();

            // Slider, leaving room for the "…" button. AlwaysClamp keeps a
            // value typed straight into the slider bounded to [min, max].
            ImGui::SetNextItemWidth(-28.0f);
            ImGui::SliderFloat("##value", &value, minVal, maxVal, "%.3f",
                               ImGuiSliderFlags_AlwaysClamp);
            widgetHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);

            // "…" button → dialog to type an exact value within [min, max].
            ImGui::SameLine();
            if (!readOnly && ImGui::Button("...", ImVec2(24.0f, 0.0f))) {
                m_dialogValue = value;
                ImGui::OpenPopup("###EditNumberDialog");
            }

            std::string caption = descriptor.getDisplayName();
            if (!descriptor.getCategory().empty()) {
                caption = descriptor.getCategory() + " / " + caption;
            }
            caption += "###EditNumberDialog";

            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                                    ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_TitleBg,       IPropertyEditor::dialogAccent());
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IPropertyEditor::dialogAccent());
            ImGui::PushStyleColor(ImGuiCol_Text,          IPropertyEditor::dialogTextColor());
            const bool numberDialogOpen = ImGui::BeginPopupModal(
                caption.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::PopStyleColor(3);
            if (numberDialogOpen) {
                // ImGui never auto-closes a modal on Escape; do it ourselves,
                // matching the Cancel button (no revert needed — the value is
                // only written back on OK).
                if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
                    && ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::Text("%.3f – %.3f", minVal, maxVal);
                ImGui::SetNextItemWidth(220.0f);
                ImGui::InputFloat("##number_input", &m_dialogValue);
                ImGui::Spacing();
                if (ImGui::Button(IPropertyEditor::tr("DIALOG.OK").c_str(), ImVec2(90, 0))) {
                    value = std::clamp(m_dialogValue, minVal, maxVal);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button(IPropertyEditor::tr("DIALOG.CANCEL").c_str(), ImVec2(90, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        } else {
            // Use drag for unconstrained or partially constrained values
            float speed = 0.1f;
            if (constraints.step.has_value()) {
                speed = constraints.step.value();
            }
            ImGui::SetNextItemWidth(-1);
            ImGui::DragFloat("##value", &value, speed, 0.0f, 0.0f, "%.3f");
            widgetHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);

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

        // Show tooltip on widget hover
        if (!descriptor.getDescription().empty() && widgetHovered) {
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
    std::string FloatEditor::getEditorId() const {
        return "FloatEditor";
    }
}
