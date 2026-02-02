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

    // =========================================================================
    // TEST INSPECTABLE - Remove this section once real entities are implemented
    // =========================================================================

    /**
     * @brief Test implementation of IInspectable for demonstration purposes
     *
     * This class demonstrates all property types supported by the inspector.
     * Remove this class once real game entities are implemented.
     */
    class TestInspectable : public Inspector::IInspectable {
    private:
        std::string m_name;
        std::string m_description;
        int m_health;
        float m_speed;
        bool m_isVisible;
        bool m_isInteractive;
        int m_selectedType;
        ImVec4 m_color;
        ImVec2 m_position;
        Inspector::PropertyEventDispatcher m_eventDispatcher;

    public:
        TestInspectable()
            : m_name("Test Entity"),
              m_description("A sample entity for testing the inspector panel"),
              m_health(100),
              m_speed(5.5f),
              m_isVisible(true),
              m_isInteractive(true),
              m_selectedType(0),
              m_color(0.2f, 0.6f, 1.0f, 1.0f),
              m_position(100.0f, 200.0f) {
        }

        std::string getTypeName() const override {
            return "Entity";
        }

        std::string getDisplayName() const override {
            return m_name;
        }

        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override {
            std::vector<Inspector::PropertyDescriptor> descriptors;

            // General category
            descriptors.push_back(
                Inspector::PropertyDescriptor("name", "Name", Inspector::PropertyType::String)
                    .setCategory("General")
                    .setDescription("The display name of this entity")
            );

            descriptors.push_back(
                Inspector::PropertyDescriptor("description", "Description", Inspector::PropertyType::String)
                    .setCategory("General")
                    .setDescription("A brief description of this entity")
            );

            descriptors.push_back(
                Inspector::PropertyDescriptor("type", "Type", Inspector::PropertyType::Enum)
                    .setCategory("General")
                    .setDescription("The type of this entity")
                    .setConstraints(Inspector::PropertyConstraints::enumeration({
                        "Character", "Item", "Scenery", "Trigger"
                    }))
            );

            // Stats category
            descriptors.push_back(
                Inspector::PropertyDescriptor("health", "Health", Inspector::PropertyType::Int)
                    .setCategory("Stats")
                    .setDescription("Current health points")
                    .setConstraints(Inspector::PropertyConstraints::numeric(0, 100, 1))
            );

            descriptors.push_back(
                Inspector::PropertyDescriptor("speed", "Speed", Inspector::PropertyType::Float)
                    .setCategory("Stats")
                    .setDescription("Movement speed multiplier")
                    .setConstraints(Inspector::PropertyConstraints::numeric(0.0f, 10.0f, 0.1f))
            );

            // Appearance category
            descriptors.push_back(
                Inspector::PropertyDescriptor("color", "Color", Inspector::PropertyType::Color)
                    .setCategory("Appearance")
                    .setDescription("The tint color of this entity")
            );

            descriptors.push_back(
                Inspector::PropertyDescriptor("visible", "Visible", Inspector::PropertyType::Bool)
                    .setCategory("Appearance")
                    .setDescription("Whether this entity is visible in the scene")
            );

            // Transform category
            descriptors.push_back(
                Inspector::PropertyDescriptor("position", "Position", Inspector::PropertyType::Vector2)
                    .setCategory("Transform")
                    .setDescription("Position in 2D space")
            );

            // Interaction category
            descriptors.push_back(
                Inspector::PropertyDescriptor("interactive", "Interactive", Inspector::PropertyType::Bool)
                    .setCategory("Interaction")
                    .setDescription("Whether the player can interact with this entity")
            );

            return descriptors;
        }

        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override {
            if (propertyId == "name") return m_name;
            if (propertyId == "description") return m_description;
            if (propertyId == "health") return m_health;
            if (propertyId == "speed") return m_speed;
            if (propertyId == "visible") return m_isVisible;
            if (propertyId == "interactive") return m_isInteractive;
            if (propertyId == "type") return Inspector::EnumValue(m_selectedType, {"Character", "Item", "Scenery", "Trigger"});
            if (propertyId == "color") return m_color;
            if (propertyId == "position") return m_position;
            return std::monostate{};
        }

        bool setPropertyValue(const std::string& propertyId, const Inspector::PropertyValue& value) override {
            if (propertyId == "name" && std::holds_alternative<std::string>(value)) {
                m_name = std::get<std::string>(value);
                return true;
            }
            if (propertyId == "description" && std::holds_alternative<std::string>(value)) {
                m_description = std::get<std::string>(value);
                return true;
            }
            if (propertyId == "health" && std::holds_alternative<int>(value)) {
                m_health = std::get<int>(value);
                return true;
            }
            if (propertyId == "speed" && std::holds_alternative<float>(value)) {
                m_speed = std::get<float>(value);
                return true;
            }
            if (propertyId == "visible" && std::holds_alternative<bool>(value)) {
                m_isVisible = std::get<bool>(value);
                return true;
            }
            if (propertyId == "interactive" && std::holds_alternative<bool>(value)) {
                m_isInteractive = std::get<bool>(value);
                return true;
            }
            if (propertyId == "type" && std::holds_alternative<Inspector::EnumValue>(value)) {
                m_selectedType = std::get<Inspector::EnumValue>(value).selectedIndex;
                return true;
            }
            if (propertyId == "color" && std::holds_alternative<ImVec4>(value)) {
                m_color = std::get<ImVec4>(value);
                return true;
            }
            if (propertyId == "position" && std::holds_alternative<ImVec2>(value)) {
                m_position = std::get<ImVec2>(value);
                return true;
            }
            return false;
        }

        Inspector::PropertyEventDispatcher& getEventDispatcher() override {
            return m_eventDispatcher;
        }
    };

    // Static test object instance
    static TestInspectable s_testObject;

    // =========================================================================
    // END TEST INSPECTABLE
    // =========================================================================

    InspectorPanel::InspectorPanel()
        : BasePanel("Inspector"),
          m_selectedObject(nullptr),
          m_needsRefresh(false) {
        // Automatically select the test object for demonstration
        setSelectedObject(&s_testObject);
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

        // Show tooltip with description on hover
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
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

        ImGui::Begin(m_windowName.c_str());

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
