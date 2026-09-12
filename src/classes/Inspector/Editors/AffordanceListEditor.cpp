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
 * @file AffordanceListEditor.cpp
 * @brief Implementation of the AffordanceListEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 */

#include "AffordanceListEditor.h"
#include "EditorLayout.h"
#include "imgui.h"
#include <algorithm>
#include <cstring>

namespace {
    constexpr size_t kNameBufSize = 128;
    constexpr size_t kTriggerBufSize = 128;

    /// Render one InputText bound to a std::string via a scratch stack buffer.
    /// Returns true if the string was edited.
    bool inputTextForString(const char* imguiId, std::string& value, size_t bufSize) {
        std::vector<char> buf(bufSize, '\0');
        std::strncpy(buf.data(), value.c_str(), bufSize - 1);
        bool edited = ImGui::InputText(imguiId, buf.data(), bufSize);
        if (edited) {
            value.assign(buf.data());
        }
        return edited;
    }
}

namespace ADS::Inspector::Editors {
    /**
     * @brief Get the property types this editor can handle
     *
     * @return std::vector<PropertyType> List of supported types
     */
    std::vector<PropertyType> AffordanceListEditor::getSupportedTypes() const {
        return { PropertyType::AffordanceList };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * One bordered row per affordance: an editable name field, a remove
     * button, and its own nested addable/removable list of trigger-name
     * fields. A footer offers a "+" to add a blank custom entry and, when
     * the descriptor supplies an options provider, a preset combo to
     * quick-add one of the known affordance names.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult AffordanceListEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        AffordanceListValue entries = getValueOr<AffordanceListValue>(currentValue, AffordanceListValue());

        std::vector<std::string> presets;
        if (const auto& provider = descriptor.getOptionsProvider()) {
            presets = provider();
        }
        std::vector<std::string> presetKeys;
        if (const auto& provider = descriptor.getPresetKeysProvider()) {
            presetKeys = provider();
        }
        // Index-paired with presets — see PropertyDescriptor::setPresetKeys().
        // A size mismatch means the descriptor is misconfigured; fall back to
        // "no keys" rather than reading out of bounds.
        if (presetKeys.size() != presets.size()) {
            presetKeys.clear();
        }

        ImGui::PushID(descriptor.getId().c_str());
        beginPropertyColumns(descriptor);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }
        ImGui::NextColumn();

        if (readOnly) {
            ImGui::BeginDisabled();
        }

        bool changed = false;
        int removeAffordance = -1;

        float rowHeight = ImGui::GetTextLineHeightWithSpacing();
        float boxHeight = std::clamp(
            rowHeight * static_cast<float>(entries.size() * 2 + 2) + 16.0f, 60.0f, 220.0f);
        ImGui::BeginChild("##affordance_list", ImVec2(-1, boxHeight), true);

        for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
            AffordanceEntry& entry = entries[static_cast<size_t>(i)];
            ImGui::PushID(i);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 28.0f);
            if (inputTextForString(
                    "##name", entry.name, kNameBufSize
                )) {
                // Hand-editing a preset-derived name detaches it from the
                // preset — it no longer re-translates on a language switch,
                // matching a custom-typed entry.
                entry.presetKey.clear();
                changed = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("x")) {
                removeAffordance = i;
            }

            ImGui::Indent();
            int removeTrigger = -1;
            for (int t = 0; t < static_cast<int>(entry.triggers.size()); ++t) {
                ImGui::PushID(t);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 28.0f);
                if (inputTextForString("##trigger", entry.triggers[static_cast<size_t>(t)], kTriggerBufSize)) {
                    changed = true;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("x")) {
                    removeTrigger = t;
                }
                ImGui::PopID();
            }
            if (removeTrigger >= 0) {
                entry.triggers.erase(entry.triggers.begin() + removeTrigger);
                changed = true;
            }
            if (ImGui::SmallButton(IPropertyEditor::tr("INSPECTOR.AFFORDANCE_ADD_TRIGGER").c_str())) {
                entry.triggers.emplace_back();
                changed = true;
            }
            ImGui::Unindent();
            ImGui::Separator();

            ImGui::PopID();
        }

        ImGui::EndChild();

        if (removeAffordance >= 0) {
            entries.erase(entries.begin() + removeAffordance);
            changed = true;
        }

        if (ImGui::SmallButton(IPropertyEditor::tr("INSPECTOR.AFFORDANCE_ADD_CUSTOM").c_str())) {
            entries.emplace_back();
            changed = true;
        }

        if (!presets.empty()) {
            ImGui::SameLine();
            ImGuiStorage* storage = ImGui::GetStateStorage();
            ImGuiID presetStorageId = ImGui::GetID("##affPresetIdx");
            int presetIdx = storage->GetInt(presetStorageId, 0);
            presetIdx = std::clamp(presetIdx, 0, static_cast<int>(presets.size()) - 1);

            std::string itemsStr;
            for (const auto& p : presets) {
                itemsStr += p;
                itemsStr += '\0';
            }
            itemsStr += '\0';

            ImGui::SetNextItemWidth(160.0f);
            ImGui::Combo("##affPresetCombo", &presetIdx, itemsStr.c_str());
            storage->SetInt(presetStorageId, presetIdx);

            ImGui::SameLine();
            if (ImGui::SmallButton(IPropertyEditor::tr("INSPECTOR.AFFORDANCE_ADD_PRESET").c_str())) {
                const std::string& chosen = presets[static_cast<size_t>(presetIdx)];
                const std::string chosenKey = presetKeys.empty()
                    ? std::string{} : presetKeys[static_cast<size_t>(presetIdx)];
                bool exists = std::any_of(entries.begin(), entries.end(),
                    [&](const AffordanceEntry& e) {
                        return chosenKey.empty() ? e.name == chosen : e.presetKey == chosenKey;
                    });
                if (!exists) {
                    entries.emplace_back(chosen, std::vector<std::string>{}, chosenKey);
                    changed = true;
                }
            }
        }

        if (readOnly) {
            ImGui::EndDisabled();
        }

        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (changed && !readOnly) {
            return EditResult::modified(entries);
        }

        return EditResult::unchanged();
    }

    /**
     * @brief Get a unique identifier for this editor type
     *
     * @return std::string Unique editor identifier
     */
    std::string AffordanceListEditor::getEditorId() const {
        return "AffordanceListEditor";
    }
}
