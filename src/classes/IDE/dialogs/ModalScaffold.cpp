/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#include "ModalScaffold.h"

#include <cfloat>
#include <string>

#include "../DesignTokens.h"

namespace ADS::IDE {

    bool beginModal(const char* strId, const std::string& title, const ModalStyle& style)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings;
        if (style.autoResize) {
            flags |= ImGuiWindowFlags_AlwaysAutoResize;
        } else {
            // The window itself never scrolls: per the scrolling-content
            // convention above, a dialog with more content than fits wraps
            // its body in its own child (`ImVec2(0, -footerHeight)`), which
            // owns the one scrollbar. Without this, a body that slightly
            // overflows would show a second, outer scrollbar alongside it.
            flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

            ImVec2 size = style.initialSize;
            if (size.y <= 0.0f) {
                size.y = 420.0f;
            }
            ImVec2 minSize = style.minSize;
            if (minSize.y <= 0.0f) {
                minSize.y = 120.0f;
            }
            ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
            ImGui::SetNextWindowSizeConstraints(minSize, ImVec2(FLT_MAX, FLT_MAX));
        }

        // Tint the genuine ImGui title bar with the same fill as the app's own
        // docked-panel caption bar (C_PANEL_TITLE), so a modal's caption reads
        // as consistent app chrome rather than a differently-colored accent.
        ImGui::PushStyleColor(ImGuiCol_TitleBg,       ADS::IDE::Colors::C_PANEL_TITLE);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ADS::IDE::Colors::C_PANEL_TITLE);
        ImGui::PushStyleColor(ImGuiCol_Text,          ADS::IDE::Colors::C_CAPTION_TEXT);

        // "Visible caption###stable-id": ImGui resets the hash at "###", so the
        // window keeps identity even when the translated caption changes.
        const std::string label = title + strId;

        // A couple of extra pixels above/below the caption text — window
        // title-bar height is derived from FramePadding.y at Begin() time —
        // so the tinted bar doesn't read as cramped against the text.
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                             ImVec2(ImGui::GetStyle().FramePadding.x, ImGui::GetStyle().FramePadding.y + 2.0f));

        if (style.captionFont != nullptr) {
            ImGui::PushFont(style.captionFont);
        }
        const bool open = ImGui::BeginPopupModal(label.c_str(), nullptr, flags);
        if (style.captionFont != nullptr) {
            ImGui::PopFont();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        return open;
    }

    void endModal()
    {
        ImGui::EndPopup();
    }

} // namespace ADS::IDE
