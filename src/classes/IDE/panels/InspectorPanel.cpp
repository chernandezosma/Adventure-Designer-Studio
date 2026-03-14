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
 * @file InspectorPanel.cpp
 * @brief Implementation of the InspectorPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "InspectorPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {

    InspectorPanel::InspectorPanel()
        : BasePanel("hInspector"),
          m_selectedObject(nullptr),
          m_needsRefresh(false) {
        m_windowTitle = this->getTranslationsManager()->_t("INSPECTOR");
    }

    void InspectorPanel::renderObjectHeader() {
        if (!m_selectedObject) return;

        // Type badge
        ImGui::TextColored(
            ImVec4(0.4f, 0.7f, 1.0f, 1.0f),
            "[%s]",
            m_selectedObject->getTypeName().c_str()
        );
        ImGui::SameLine();

        // Object name
        ImGui::Text("%s", m_selectedObject->getDisplayName().c_str());
    }

    void InspectorPanel::renderCategory(
        const std::string& category,
        const std::vector<Inspector::PropertyDescriptor>& properties
    ) {
        // Use collapsing header for category grouping
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (ImGui::CollapsingHeader(category.c_str(), flags)) {
            ImGui::Indent(10.0f);

            for (const auto& descriptor : properties) {
                // Check visibility condition
                if (!descriptor.isVisible(m_selectedObject)) {
                    continue;
                }

                renderProperty(descriptor);
            }

            ImGui::Unindent(10.0f);
        }
    }

    void InspectorPanel::renderProperty(const Inspector::PropertyDescriptor& descriptor) {
        // Get current value
        Inspector::PropertyValue currentValue =
            m_selectedObject->getPropertyValue(descriptor.getId());

        // Get appropriate editor
        Inspector::Editors::IPropertyEditor* editor =
            m_editorRegistry.getEditorForProperty(descriptor);

        if (!editor) {
            // No editor available, show read-only text
            ImGui::TextDisabled("%s: (no editor)", descriptor.getDisplayName().c_str());
            return;
        }

        // Render editor and handle value changes
        Inspector::Editors::EditResult result = editor->render(
            descriptor,
            currentValue,
            descriptor.isReadOnly()
        );

        // Apply change if value was modified
        if (result.changed) {
            m_selectedObject->setPropertyValue(descriptor.getId(), result.newValue);
        }
    }

    void InspectorPanel::refreshCategoryCache() {
        m_categorizedProperties.clear();

        if (!m_selectedObject) return;

        // Get all property descriptors
        std::vector<Inspector::PropertyDescriptor> descriptors =
            m_selectedObject->getPropertyDescriptors();

        // Group by category
        for (const auto& descriptor : descriptors) {
            std::string category = descriptor.getCategory();
            if (category.empty()) {
                category = "General";  // Default category
            }
            m_categorizedProperties[category].push_back(descriptor);
        }
    }

    void InspectorPanel::renderNoSelection() {
        ImGui::TextColored(
            ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Select an entity to view and edit its properties"
        );
    }

    void InspectorPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(getImGuiLabel().c_str());

        if (!m_selectedObject) {
            renderNoSelection();
            ImGui::End();
            return;
        }

        // Refresh cache if needed
        if (m_needsRefresh) {
            refreshCategoryCache();
            m_needsRefresh = false;
        }

        // Render object header
        renderObjectHeader();
        ImGui::Separator();
        ImGui::Spacing();

        // Render properties by category
        for (const auto& [category, properties] : m_categorizedProperties) {
            renderCategory(category, properties);
            ImGui::Spacing();
        }

        ImGui::End();
    }

    void InspectorPanel::setSelectedObject(Inspector::IInspectable* object) {
        if (m_selectedObject != object) {
            m_selectedObject = object;
            m_needsRefresh = true;
            refreshCategoryCache();  // Refresh immediately
        }
    }

    Inspector::IInspectable* InspectorPanel::getSelectedObject() const {
        return m_selectedObject;
    }

    void InspectorPanel::clearSelection() {
        setSelectedObject(nullptr);
    }

    void InspectorPanel::refresh() {
        m_needsRefresh = true;
    }
}
