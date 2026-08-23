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
 * @file StringEditor.cpp
 * @brief Implementation of the StringEditor class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "StringEditor.h"
#include "EditorLayout.h"
#include "TextReflow.h"
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include "languages.h"
#include <cfloat>
#include <cstring>
#include <algorithm>

namespace ADS::Inspector::Editors {
    /**
     * @brief Construct a new StringEditor
     */
    StringEditor::StringEditor() {
        std::memset(m_buffer, 0, sizeof(m_buffer));
        std::memset(m_dialogBuffer, 0, sizeof(m_dialogBuffer));
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
        if (descriptor.getConstraints().translatable) {
            return renderTranslatable(descriptor, currentValue, readOnly);
        }

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
        bool changed = false;
        if (readOnly) {
            ImGui::BeginDisabled();
        }

        bool multiline  = descriptor.getConstraints().multiline;
        bool isFilePath = descriptor.getConstraints().isFilePath;
        bool browseRequested = false;

        // Leave room for the "…"/"Browse…" button on multiline or file-path fields
        ImGui::SetNextItemWidth((multiline || isFilePath) ? -28.0f : -1.0f);
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

        // Captured on the input widget itself, before any trailing button —
        // ImGui::IsItemHovered() only reports the immediately preceding item,
        // so a check after the "…"/browse button would describe the button.
        const bool widgetHovered =
            ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);

        // Warn (with a trailing "…") when the value is wider than the field.
        drawInputOverflowHint(m_buffer);

        if (multiline) {
            ImGui::SameLine();
            if (ImGui::Button("...", ImVec2(24.0f, 0.0f))) {
                std::strncpy(m_dialogBuffer, m_buffer, sizeof(m_dialogBuffer) - 1);
                m_dialogBuffer[sizeof(m_dialogBuffer) - 1] = '\0';
                ImGui::OpenPopup("###EditTextDialog");
            }
            // Caption is the field's category + name ("Descriptions / Normal")
            // so a short label like "Normal" is not ambiguous on its own.
            std::string dialogCaption = descriptor.getDisplayName();
            if (!descriptor.getCategory().empty()) {
                dialogCaption = descriptor.getCategory() + " / " + dialogCaption;
            }
            const std::string dialogLabel = dialogCaption + "###EditTextDialog";
            ImGui::SetNextWindowPos(
                ImGui::GetMainViewport()->GetCenter(),
                ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSizeConstraints(ImVec2(420.0f, 300.0f), ImVec2(FLT_MAX, FLT_MAX));
            ImGui::SetNextWindowSize(ImVec2(620.0f, 460.0f), ImGuiCond_Appearing);
            ImGui::PushStyleColor(ImGuiCol_TitleBg,       IPropertyEditor::dialogAccent());
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IPropertyEditor::dialogAccent());
            ImGui::PushStyleColor(ImGuiCol_Text,          IPropertyEditor::dialogTextColor());
            const bool textDialogOpen = ImGui::BeginPopupModal(dialogLabel.c_str(), nullptr, 0);
            ImGui::PopStyleColor(3);
            if (textDialogOpen) {
                // ImGui never auto-closes a modal on Escape; do it ourselves,
                // matching Cancel (the buffer is only copied back on OK).
                if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
                    && ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
                    ImGui::CloseCurrentPopup();
                }
                float textHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();
                const float boxW = ImGui::GetContentRegionAvail().x;
                float wrapPx = boxW
                    - ImGui::GetStyle().FramePadding.x * 2.0f
                    - ImGui::GetStyle().ScrollbarSize - 2.0f;
                // NoHorizontalScroll pins the view; the CallbackAlways hook
                // rewraps the buffer in place every frame so the text folds to
                // the box width as it is typed instead of scrolling right.
                ImGui::InputTextMultiline(
                    "##dialog_value", m_dialogBuffer,
                    std::min(sizeof(m_dialogBuffer), maxLen + 1),
                    ImVec2(boxW, textHeight),
                    ImGuiInputTextFlags_NoHorizontalScroll
                        | ImGuiInputTextFlags_CallbackAlways,
                    &wrapCallback, &wrapPx);
                // The callback only runs while the field is active; rewrap once
                // more when idle so resizing the dialog reflows the text too.
                if (!ImGui::IsItemActive()) {
                    wrapInPlace(m_dialogBuffer,
                                static_cast<int>(std::strlen(m_dialogBuffer)), wrapPx);
                }
                if (ImGui::Button(IPropertyEditor::tr("DIALOG.OK").c_str())) {
                    unwrapSoft(m_dialogBuffer, sizeof(m_dialogBuffer));
                    std::strncpy(m_buffer, m_dialogBuffer, maxLen);
                    m_buffer[maxLen] = '\0';
                    changed = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button(IPropertyEditor::tr("DIALOG.CANCEL").c_str())) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        } else if (isFilePath) {
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_FOLDER_OPEN, ImVec2(24.0f, 0.0f))) {
                browseRequested = true;
            }
        }

        if (readOnly) {
            ImGui::EndDisabled();
        }

        // Show tooltip on widget hover (captured on the input, not the button)
        if (!descriptor.getDescription().empty() && widgetHovered) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        // Requesting a file dialog takes priority — no value change happens
        // this frame; InspectorPanel queues it for after SDL_RenderPresent
        // (see CLAUDE.md's deferred dialog pattern).
        if (browseRequested && !readOnly) {
            return EditResult::requestFileDialog(descriptor.getConstraints().fileExtensions);
        }

        if (changed && std::string(m_buffer) != currentStr) {
            return EditResult::modified(std::string(m_buffer));
        }

        return EditResult::unchanged();
    }

    /**
     * @brief Render a translatable String property
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param descriptor Property metadata
     * @param currentValue Current value — expected to hold LocalizedText
     * @param readOnly If true, the "…" button is disabled
     * @return EditResult New LocalizedText map on change (OK), unchanged on Cancel
     */
    EditResult StringEditor::renderTranslatable(
        const PropertyDescriptor& descriptor,
        const PropertyValue& currentValue,
        bool readOnly
    ) {
        LocalizedText texts = getValueOr<LocalizedText>(currentValue, LocalizedText{});
        const std::string defaultLang(Constants::Languages::DEFAULT_FALLBACK);

        std::string previewText;
        if (auto it = texts.find(defaultLang); it != texts.end()) {
            previewText = it->second;
        }

        // Buffer bound for the inline field — honour the maxLength constraint.
        size_t maxLen = DEFAULT_BUFFER_SIZE - 1;
        if (descriptor.getConstraints().hasStringConstraints()) {
            maxLen = std::min(
                descriptor.getConstraints().maxLength.value(),
                DEFAULT_BUFFER_SIZE - 1
            );
        }

        std::strncpy(m_buffer, previewText.c_str(), maxLen);
        m_buffer[maxLen] = '\0';

        ImGui::PushID(descriptor.getId().c_str());
        beginPropertyColumns(descriptor);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", descriptor.getDisplayName().c_str());
        if (!descriptor.getDescription().empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }
        ImGui::NextColumn();

        bool changed = false;

        // Editable inline field for the default language. Multi-language
        // authoring is still the Translations panel's job (View > Translations);
        // here the author edits the base text directly, and the "…" dialog
        // gives room for longer copy.
        if (readOnly) ImGui::BeginDisabled();
        ImGui::SetNextItemWidth(-28.0f);
        if (ImGui::InputText("##value", m_buffer, maxLen + 1,
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            changed = true;
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            changed = true;
        }
        const bool widgetHovered =
            ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal);
        drawInputOverflowHint(m_buffer);

        ImGui::SameLine();
        bool openDialog = false;
        if (ImGui::Button("...", ImVec2(24.0f, 0.0f))) {
            openDialog = true;
        }
        if (readOnly) ImGui::EndDisabled();

        if (openDialog && !readOnly) {
            m_dialogLocalizedText = texts;
            m_dialogLanguage = defaultLang;
            // Seed from the live inline buffer so text typed but not yet
            // committed this frame still shows up in the dialog.
            std::strncpy(m_dialogBuffer, m_buffer, sizeof(m_dialogBuffer) - 1);
            m_dialogBuffer[sizeof(m_dialogBuffer) - 1] = '\0';
            ImGui::OpenPopup("###EditTranslatableDialog");
        }

        if (changed) {
            m_dialogLocalizedText = texts;
            m_dialogLocalizedText[defaultLang] = std::string(m_buffer);
        }

        // Caption is the field's category + name ("Descriptions / Normal") so
        // a short label like "Normal" is not ambiguous on its own.
        std::string dialogCaption = descriptor.getDisplayName();
        if (!descriptor.getCategory().empty()) {
            dialogCaption = descriptor.getCategory() + " / " + dialogCaption;
        }
        const std::string dialogLabel = dialogCaption + "###EditTranslatableDialog";
        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(),
            ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(420.0f, 300.0f), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::SetNextWindowSize(ImVec2(620.0f, 460.0f), ImGuiCond_Appearing);
        ImGui::PushStyleColor(ImGuiCol_TitleBg,       IPropertyEditor::dialogAccent());
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IPropertyEditor::dialogAccent());
            ImGui::PushStyleColor(ImGuiCol_Text,          IPropertyEditor::dialogTextColor());
        const bool trDialogOpen = ImGui::BeginPopupModal(dialogLabel.c_str(), nullptr, 0);
        ImGui::PopStyleColor(3);
        if (trDialogOpen) {
            // ImGui never auto-closes a modal on Escape; do it ourselves,
            // matching Cancel (changes are only kept on OK).
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
                && ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
                ImGui::CloseCurrentPopup();
            }
            // The target language is the project's default (m_dialogLanguage,
            // fixed on open). Multi-language authoring is done in the dedicated
            // Translations panel (View > Translations), not here.
            float textHeight = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();
            const float boxW = ImGui::GetContentRegionAvail().x;
            float wrapPx = boxW
                - ImGui::GetStyle().FramePadding.x * 2.0f
                - ImGui::GetStyle().ScrollbarSize - 2.0f;
            // NoHorizontalScroll pins the view; the CallbackAlways hook rewraps
            // the buffer in place every frame so the text folds to the box
            // width as it is typed instead of scrolling right.
            ImGui::InputTextMultiline(
                "##dialog_value", m_dialogBuffer,
                std::min(sizeof(m_dialogBuffer), maxLen + 1),
                ImVec2(boxW, textHeight),
                ImGuiInputTextFlags_NoHorizontalScroll
                    | ImGuiInputTextFlags_CallbackAlways,
                &wrapCallback, &wrapPx);
            // The callback only runs while the field is active; rewrap once
            // more when idle so resizing the dialog reflows the text too.
            if (!ImGui::IsItemActive()) {
                wrapInPlace(m_dialogBuffer,
                            static_cast<int>(std::strlen(m_dialogBuffer)), wrapPx);
            }

            if (ImGui::Button(IPropertyEditor::tr("DIALOG.OK").c_str())) {
                unwrapSoft(m_dialogBuffer, sizeof(m_dialogBuffer));
                m_dialogLocalizedText = texts;
                m_dialogLocalizedText[m_dialogLanguage] =
                    std::string(m_dialogBuffer).substr(0, maxLen);
                changed = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(IPropertyEditor::tr("DIALOG.CANCEL").c_str())) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (!descriptor.getDescription().empty() && widgetHovered) {
            ImGui::SetTooltip("%s", descriptor.getDescription().c_str());
        }

        ImGui::Columns(1);
        ImGui::PopID();

        if (changed) {
            return EditResult::modified(m_dialogLocalizedText);
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