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
 * @file StringEditor.cpp
 * @brief Implementation of the StringEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "StringEditor.h"
#include "imgui.h"
#include <cstring>
#include <algorithm>

namespace ADS::Inspector::Editors {
    /**
     * @brief Construct a new StringEditor
     */
    StringEditor::StringEditor() {
        std::memset(m_buffer, 0, sizeof(m_buffer));
    }

    /**
     * @brief Get the property types this editor can handle
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @return std::vector<PropertyType> List of supported types
     */
    std::vector<PropertyType> StringEditor::getSupportedTypes() const {
        return { PropertyType::String };
    }

    /**
     * @brief Render the editor for a property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a text input field for editing string properties.
     * Respects maxLength constraints if specified.
     *
     * @param descriptor Property metadata
     * @param currentValue Current property value
     * @param readOnly True if the property cannot be edited
     * @return EditResult Result of the edit operation
     */
    EditResult StringEditor::render(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        // Get current string value
        std::string currentStr = getValueOr<std::string>(currentValue, "");

        // Determine buffer size based on constraints
        size_t maxLen = DEFAULT_BUFFER_SIZE - 1;
        if (descriptor.getConstraints().hasStringConstraints()) {
            maxLen = std::min(
                descriptor.getConstraints().maxLength.value(),
                DEFAULT_BUFFER_SIZE - 1
            );
        }

        // Copy current value to buffer
        std::strncpy(m_buffer, currentStr.c_str(), maxLen);
        m_buffer[maxLen] = '\0';

        // Generate unique ID for ImGui
        ImGui::PushID(descriptor.getId().c_str());

        // Two-column layout: label on left, widget on right
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 120.0f);

        // Label column
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        ImGui::NextColumn();

        // Widget column
        bool changed = false;
        if (readOnly) {
            ImGui::BeginDisabled();
        }

        ImGui::SetNextItemWidth(-1);
        // Render input field
        if (ImGui::InputText(
            "##value",
            m_buffer,
            maxLen + 1,
            ImGuiInputTextFlags_EnterReturnsTrue
        )) {
            changed = true;
        }

        // Also check for deactivation (user finished editing)
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            changed = true;
        }

        if (readOnly) {
            ImGui::EndDisabled();
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (changed && std::string(m_buffer) != currentStr) {
            return EditResult::modified(std::string(m_buffer));
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
    std::string StringEditor::getEditorId() const {
        return "StringEditor";
    }
}