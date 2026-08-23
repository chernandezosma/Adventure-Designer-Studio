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
 * @file SelectEditor.cpp
 * @brief Implementation of the SelectEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include "SelectEditor.h"
#include "EditorLayout.h"
#include "imgui.h"
#include <algorithm>
#include <cstdint>

namespace ADS::Inspector::Editors {
    /**
     * @brief Get the property types this editor can handle
     *
     * @return std::vector<PropertyType> List of supported types
     */
    std::vector<PropertyType> SelectEditor::getSupportedTypes() const {
        return { PropertyType::Select };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Single-select: an ImGui::Combo with a synthetic leading "(None)"
     * entry so the field can represent "no selection". Multi-select: a
     * bordered checklist, one checkbox per option.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult SelectEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        SelectValue selectVal = getValueOr<SelectValue>(currentValue, SelectValue());

        // Resolve options from the dynamic provider, falling back to
        // static enum constraints (mirrors EnumEditor's fallback).
        std::vector<std::string> options = selectVal.options;
        if (options.empty()) {
            const auto& provider = descriptor.getOptionsProvider();
            if (provider) {
                options = provider();
            } else if (descriptor.getConstraints().hasEnumConstraints()) {
                options = descriptor.getConstraints().enumValues;
            }
        }

        ImGui::PushID(descriptor.getId().c_str());
        beginPropertyColumns(descriptor);

        // Label column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }
        ImGui::NextColumn();

        // Widget column
        if (options.empty()) {
            ImGui::TextDisabled(
                "%s", IPropertyEditor::tr(descriptor.getEmptyOptionsTextKey()).c_str());
            bool createClicked = false;
            if (descriptor.isAllowCreateNew()) {
                ImGui::SameLine();
                createClicked = ImGui::SmallButton("+");
            }
            ImGui::Columns(1);
            ImGui::PopID();
            if (createClicked) {
                return EditResult::requestCreateNew();
            }
            return EditResult::unchanged();
        }

        if (readOnly) {
            ImGui::BeginDisabled();
        }

        bool changed = false;
        bool createNewRequested = false;
        std::vector<int> newIndices = selectVal.selectedIndices;

        // Captured right after the value widget itself (Combo, or the
        // multi-select checklist), before the "+" button is rendered —
        // IsItemHovered() only ever reports on the *immediately preceding*
        // item, so checking it after the button (as this used to) reported
        // whether the button was hovered, never the widget, and the
        // description tooltip silently never showed for any field with the
        // "+" button (PropertyDescriptor::isAllowCreateNew()).
        bool valueHovered = false;

        // Set when the user just checked ON an option marked user-defined
        // (PropertyDescriptor::isUserDefinedOption) — the checkbox is not
        // committed this frame; InspectorPanel prompts for a label first.
        bool labelDialogRequested = false;
        uint8_t labelDialogBitPosition = 0;
        std::vector<int> labelDialogPendingIndices;

        if (descriptor.isMultiSelect()) {
            float rowHeight = ImGui::GetTextLineHeightWithSpacing();
            float boxHeight = std::min(rowHeight * static_cast<float>(options.size()) + 8.0f, 140.0f);
            ImGui::BeginChild("##select_list", ImVec2(-1, boxHeight), true);
            for (int i = 0; i < static_cast<int>(options.size()); ++i) {
                bool checked = std::find(newIndices.begin(), newIndices.end(), i) != newIndices.end();
                if (ImGui::Checkbox(options[i].c_str(), &checked)) {
                    if (checked && descriptor.isUserDefinedOption(i)) {
                        labelDialogRequested = true;
                        labelDialogBitPosition = descriptor.getUserDefinedOptionBit(i);
                        labelDialogPendingIndices = newIndices;
                        labelDialogPendingIndices.push_back(i);
                        std::sort(labelDialogPendingIndices.begin(), labelDialogPendingIndices.end());
                    } else {
                        changed = true;
                        if (checked) {
                            newIndices.push_back(i);
                        } else {
                            newIndices.erase(std::remove(newIndices.begin(), newIndices.end(), i), newIndices.end());
                        }
                    }
                }
            }
            ImGui::EndChild();
            valueHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);
            std::sort(newIndices.begin(), newIndices.end());
        } else {
            // Synthetic leading "(None)" entry for nullable single-select.
            std::string itemsStr = "(None)";
            itemsStr += '\0';
            for (const auto& opt : options) {
                itemsStr += opt;
                itemsStr += '\0';
            }
            itemsStr += '\0';

            int comboIndex = selectVal.selectedIndices.empty() ? 0 : selectVal.selectedIndices.front() + 1;
            if (comboIndex < 0 || comboIndex > static_cast<int>(options.size())) {
                comboIndex = 0;
            }
            int originalCombo = comboIndex;

            // Leave room for the "+" button (if any) so it doesn't get
            // pushed past the panel's right edge by a full-width combo.
            // Floored at 40px: Dear ImGui gives negative SetNextItemWidth()
            // values a "distance from the right edge" meaning rather than
            // "shrink by this much" — on a narrow panel, an unclamped
            // negative width here silently made the combo render *wider*
            // than intended and pushed the "+" button off past the visible
            // edge instead of narrowing it. See InspectorPanel::
            // computeMinPanelWidth(), which keeps the panel itself from
            // ever getting this narrow in the first place.
            float comboWidth = -1.0f;
            if (descriptor.isAllowCreateNew()) {
                float reserved = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.x;
                comboWidth = std::max(ImGui::GetContentRegionAvail().x - reserved, 40.0f);
            }
            ImGui::SetNextItemWidth(comboWidth);
            ImGui::Combo("##value", &comboIndex, itemsStr.c_str());
            valueHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);

            if (comboIndex != originalCombo) {
                changed = true;
                newIndices = comboIndex == 0 ? std::vector<int>{} : std::vector<int>{comboIndex - 1};
            }
        }

        if (readOnly) {
            ImGui::EndDisabled();
        }

        if (!descriptor.isMultiSelect() && descriptor.isAllowCreateNew()) {
            ImGui::SameLine();
            if (ImGui::SmallButton("+")) {
                createNewRequested = true;
            }
        }

        if (!descriptor.getDescription().empty() && valueHovered) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (labelDialogRequested) {
            return EditResult::requestLabelDialog(
                labelDialogBitPosition, SelectValue(labelDialogPendingIndices, options));
        }

        if (createNewRequested) {
            return EditResult::requestCreateNew();
        }

        if (changed) {
            return EditResult::modified(SelectValue(newIndices, options));
        }

        return EditResult::unchanged();
    }

    /**
     * @brief Get a unique identifier for this editor type
     *
     * @return std::string Unique editor identifier
     */
    std::string SelectEditor::getEditorId() const {
        return "SelectEditor";
    }
}
