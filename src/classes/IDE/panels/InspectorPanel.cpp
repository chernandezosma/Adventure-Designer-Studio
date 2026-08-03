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

#include "InspectorPanel.h"
#include "IDE/DesignTokens.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace ADS::IDE::Panels {

using namespace ADS::IDE::Colors;

InspectorPanel::InspectorPanel()
    : BasePanel("hInspector"),
      m_selectedObject(nullptr),
      m_needsRefresh(false)
{
    m_windowTitle = this->getTranslationsManager()->_t("INSPECTOR");
}

// ---------------------------------------------------------------------------
// renderObjectHeader
// ---------------------------------------------------------------------------

void InspectorPanel::renderObjectHeader()
{
    if (!m_selectedObject) return;

    // Type badge
    ImGui::PushStyleColor(ImGuiCol_Text, C_SCENE);
    ImGui::Text("[%s]", m_selectedObject->getTypeName().c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 6);
    ImGui::TextColored(TEXT0, "%s", m_selectedObject->getDisplayName().c_str());
}

// ---------------------------------------------------------------------------
// renderCategory
// ---------------------------------------------------------------------------

void InspectorPanel::renderCategory(
    const std::string& category,
    const std::vector<Inspector::PropertyDescriptor>& properties)
{
    ImGui::PushStyleColor(ImGuiCol_Header,        BG3);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, withAlpha(C_SCENE, 0.20f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  withAlpha(C_SCENE, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_Text,          TEXT0);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen
                             | ImGuiTreeNodeFlags_SpanAvailWidth
                             | ImGuiTreeNodeFlags_FramePadding;

    bool open = ImGui::CollapsingHeader(category.c_str(), flags);

    ImGui::PopStyleColor(4);

    if (open) {
        ImGui::Indent(8.0f);
        for (const auto& descriptor : properties) {
            if (!descriptor.isVisible(m_selectedObject))
                continue;
            renderProperty(descriptor);
        }
        ImGui::Unindent(8.0f);
        ImGui::Spacing();
    }
}

// ---------------------------------------------------------------------------
// renderProperty
// ---------------------------------------------------------------------------

void InspectorPanel::renderProperty(const Inspector::PropertyDescriptor& descriptor)
{
    Inspector::PropertyValue currentValue =
        m_selectedObject->getPropertyValue(descriptor.getId());

    Inspector::Editors::IPropertyEditor* editor =
        m_editorRegistry.getEditorForProperty(descriptor);

    if (!editor) {
        ImGui::TextDisabled("%s: (no editor)", descriptor.getDisplayName().c_str());
        return;
    }

    // Pre-compute the widget's ImGuiID. All editors call PushID(descriptor.getId())
    // and then name their primary widget "##value", so we can predict the ID without
    // rendering anything. This lets us check g.ActiveId after the render.
    ImGui::PushID(descriptor.getId().c_str());
    ImGuiID widgetId = ImGui::GetID("##value");
    ImGui::PopID();

    // Apply design-token styling to all input widgets
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        BG1);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, BG2);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  BG3);
    ImGui::PushStyleColor(ImGuiCol_Border,         BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));

    Inspector::Editors::EditResult result = editor->render(
        descriptor, currentValue, descriptor.isReadOnly());

    // After the editor returns, g.LastItemData still holds the main widget (column 1).
    // Capture the rect BEFORE popping styles (rect data is independent of style state).
    ImVec2 widgetMin = ImGui::GetItemRectMin();
    ImVec2 widgetMax = ImGui::GetItemRectMax();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);

    // Overdraw a BORDER_HI rectangle around just the widget when it is active.
    // g.ActiveId persists for the duration of the interaction (typing, dragging),
    // and widgetId was pre-computed to match exactly this editor's "##value" widget.
    ImGuiContext& g = *ImGui::GetCurrentContext();
    if (g.ActiveId == widgetId) {
        ImGui::GetWindowDrawList()->AddRect(
            widgetMin, widgetMax,
            ImGui::ColorConvertFloat4ToU32(BORDER_HI),
            4.0f, 0, 1.5f
        );
    }

    if (result.changed) {
        m_selectedObject->setPropertyValue(descriptor.getId(), result.newValue);
        if (onPropertyChanged) onPropertyChanged();
    }
}

// ---------------------------------------------------------------------------
// refreshCategoryCache
// ---------------------------------------------------------------------------

void InspectorPanel::refreshCategoryCache()
{
    m_categorizedProperties.clear();
    if (!m_selectedObject) return;

    for (const auto& descriptor : m_selectedObject->getPropertyDescriptors()) {
        std::string cat = descriptor.getCategory();
        if (cat.empty()) cat = "General";
        m_categorizedProperties[cat].push_back(descriptor);
    }
}

// ---------------------------------------------------------------------------
// renderNoSelection
// ---------------------------------------------------------------------------

void InspectorPanel::renderNoSelection()
{
    ImGui::Spacing();
    float w = ImGui::GetContentRegionAvail().x;
    const char* msg = "Selecciona una entidad";
    ImGui::SetCursorPosX((w - ImGui::CalcTextSize(msg).x) * 0.5f);
    ImGui::TextColored(TEXT2, "%s", msg);
}

// ---------------------------------------------------------------------------
// render
// ---------------------------------------------------------------------------

void InspectorPanel::render()
{
    if (!m_isVisible) return;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, BG1);
    ImGui::Begin(getImGuiLabel().c_str());
    ImGui::PopStyleColor();

    if (!m_selectedObject) {
        renderNoSelection();
        ImGui::End();
        return;
    }

    if (m_needsRefresh) {
        refreshCategoryCache();
        m_needsRefresh = false;
    }

    renderObjectHeader();
    ImGui::Separator();
    ImGui::Spacing();

    // Scrollable property area
    ImGui::BeginChild("##inspector_scroll", ImVec2(0, 0), false);

    for (const auto& [category, properties] : m_categorizedProperties)
        renderCategory(category, properties);

    ImGui::EndChild();

    ImGui::End();
}

// ---------------------------------------------------------------------------
// Selection management
// ---------------------------------------------------------------------------

void InspectorPanel::setSelectedObject(Inspector::IInspectable* object)
{
    if (m_selectedObject != object) {
        m_selectedObject = object;
        m_needsRefresh = true;
        refreshCategoryCache();
    }
}

Inspector::IInspectable* InspectorPanel::getSelectedObject() const
{
    return m_selectedObject;
}

void InspectorPanel::clearSelection()
{
    setSelectedObject(nullptr);
}

void InspectorPanel::refresh()
{
    m_needsRefresh = true;
}

} // namespace ADS::IDE::Panels
