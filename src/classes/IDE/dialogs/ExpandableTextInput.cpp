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

#include "ExpandableTextInput.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstring>

#include "imgui.h"

#include "IDE/DesignTokens.h"
#include "Inspector/Editors/TextReflow.h"
#include "ModalScaffold.h"

namespace ADS::IDE {

    bool expandableTextInput(const char* strId, char* buf, std::size_t bufSize,
                             const std::string& dialogTitle,
                             const char* okLabel, const char* cancelLabel,
                             std::size_t maxLen)
    {
        // Snapshot of the text at the moment the dialog opened, so Cancel can
        // restore it. Safe as a static: only one modal is ever open at a time.
        static std::string s_backup;

        // Effective buffer bound: the caller's capacity, tightened to maxLen+1
        // when a character cap was requested.
        const std::size_t editSize =
            (maxLen > 0) ? std::min(bufSize, maxLen + 1) : bufSize;

        bool changed = false;

        // --- inline field + expand button -----------------------------------
        ImGui::SetNextItemWidth(-28.0f);
        ImGui::InputText(strId, buf, editSize);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            changed = true;
        }

        // Trailing "…" hint when the value is wider than the inline field.
        if (!ImGui::IsItemActive() && buf[0] != '\0') {
            const ImVec2 rectMin = ImGui::GetItemRectMin();
            const ImVec2 rectMax = ImGui::GetItemRectMax();
            const float  padX    = ImGui::GetStyle().FramePadding.x;
            if (ImGui::CalcTextSize(buf).x > (rectMax.x - rectMin.x) - padX * 2.0f) {
                const ImVec2 dots = ImGui::CalcTextSize("…");
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(rectMax.x - padX - dots.x,
                           rectMin.y + (rectMax.y - rectMin.y - dots.y) * 0.5f),
                    ImGui::GetColorU32(ImGuiCol_Text), "…");
            }
        }

        // "Visible caption###stable-id": ImGui resets the id hash at "###", so
        // the popup keeps identity across translated captions and the real
        // (theme-tinted) title bar shows dialogTitle.
        const std::string popupSuffix = std::string("###exp_") + strId;
        const std::string popupLabel  = dialogTitle + popupSuffix;

        ImGui::SameLine();
        if (ImGui::Button("...", ImVec2(24.0f, 0.0f))) {
            s_backup = buf;
            ImGui::OpenPopup(popupSuffix.c_str());
        }

        // --- expand dialog ------------------------------------------------
        const ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(620.0f, 460.0f), ImGuiCond_Appearing);
        ImGui::SetNextWindowSizeConstraints(ImVec2(420.0f, 300.0f), ImVec2(FLT_MAX, FLT_MAX));

        ImGui::PushStyleColor(ImGuiCol_TitleBg,       Colors::C_DIALOG_TITLE);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Colors::C_DIALOG_TITLE);
        ImGui::PushStyleColor(ImGuiCol_Text,          Colors::C_CAPTION_TEXT);
        const bool popupOpen = ImGui::BeginPopupModal(popupLabel.c_str(), nullptr, 0);
        ImGui::PopStyleColor(3);
        if (popupOpen) {
            // Escape aborts the dialog, same as the Cancel button: restore the
            // text captured when it opened, then close.
            if (modalEscapeRequested()) {
                std::snprintf(buf, bufSize, "%s", s_backup.c_str());
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return changed;
            }

            const float bodyH = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();
            const float boxW  = ImGui::GetContentRegionAvail().x;
            float wrapPx = boxW
                - ImGui::GetStyle().FramePadding.x * 2.0f
                - ImGui::GetStyle().ScrollbarSize - 2.0f;
            // NoHorizontalScroll pins the view; the CallbackAlways hook rewraps
            // the buffer in place every frame so the text folds to the box
            // width as it is typed instead of scrolling right.
            ImGui::InputTextMultiline("##exp_body", buf, editSize,
                                      ImVec2(boxW, bodyH),
                                      ImGuiInputTextFlags_NoHorizontalScroll
                                          | ImGuiInputTextFlags_CallbackAlways,
                                      &Inspector::Editors::wrapCallback, &wrapPx);
            // The callback only runs while the field is active; rewrap once
            // more when idle so resizing the dialog reflows the text too.
            if (!ImGui::IsItemActive()) {
                Inspector::Editors::wrapInPlace(
                    buf, static_cast<int>(std::strlen(buf)), wrapPx);
            }

            if (ImGui::Button(okLabel, ImVec2(90.0f, 0.0f))) {
                Inspector::Editors::unwrapSoft(buf, bufSize);
                changed = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(cancelLabel, ImVec2(90.0f, 0.0f))) {
                std::snprintf(buf, bufSize, "%s", s_backup.c_str());
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        return changed;
    }

} // namespace ADS::IDE
