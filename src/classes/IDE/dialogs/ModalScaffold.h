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

#ifndef ADS_IDE_MODAL_SCAFFOLD_H
#define ADS_IDE_MODAL_SCAFFOLD_H

/**
 * @file ModalScaffold.h
 * @brief Shared chrome (themed title bar + sizing) for every ADS modal dialog
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <string>

#include "imgui.h"

namespace ADS::IDE {

    /**
     * @brief Size and font options for a scaffolded modal.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    struct ModalStyle {
        ImVec2  initialSize = {520.0f, 0.0f};   ///< Applied once on first appear; ignored when autoResize
        ImVec2  minSize     = {320.0f, 0.0f};   ///< Resize floor; ignored when autoResize
        bool    autoResize  = false;            ///< true => AlwaysAutoResize (short, non-scrolling dialogs)
        ImFont* captionFont = nullptr;          ///< Optional heavier face for the title-bar text (e.g. "mediumFont")
    };

    /**
     * @brief Open a modal popup with a real, accent-blue ImGui title bar.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Uses a genuine ImGui window title bar tinted with @c Colors::C_DIALOG_TITLE,
     * so ImGui owns its full-width extent, the scrollbar position (below the bar)
     * and content clipping while scrolling. Centres on first appear; unless
     * @p style.autoResize the window gets a fixed initial size the user can
     * drag-resize but that never auto-grows with content.
     *
     * The visible caption comes from @p title; window identity is stable across
     * translation changes because the internal label is `title + strId` and
     * @p strId must begin with "###" (ImGui's "id = everything after ###" rule).
     * Arm the popup elsewhere with `ImGui::OpenPopup(strId)` — the same @p strId.
     *
     * Dialogs with enough content to scroll should wrap their body in an
     * `ImGui::BeginChild(..., ImVec2(0, -footerHeight))` so the button row stays
     * pinned and only the body scrolls.
     *
     * @param strId  Stable popup id, must start with "###" (e.g. "###ads_new_project")
     * @param title  Already-translated caption text
     * @param style  Size / font options
     * @return bool  true when the modal is open and its body should be drawn;
     *               when false, do NOT call endModal()
     */
    bool beginModal(const char* strId, const std::string& title, const ModalStyle& style = {});

    /**
     * @brief True on the frame Escape is pressed while the modal is focused.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Dear ImGui deliberately never auto-closes @c BeginPopupModal windows on
     * Escape (see @c NavUpdateCancelRequest, which skips
     * @c ImGuiWindowFlags_Modal), so every ADS dialog polls this once inside
     * its body and routes a true result through the same path as its Cancel
     * button — usually just @c ImGui::CloseCurrentPopup(). The @c IsKeyPressed
     * query ignores key ownership, so it also fires while a text field inside
     * the dialog is being edited: Escape then aborts the whole dialog.
     *
     * @return bool true when the open modal should close now
     */
    inline bool modalEscapeRequested()
    {
        return ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
            && ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    }

    /**
     * @brief Close a modal opened with beginModal().
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Call exactly once, only when beginModal() returned true.
     */
    void endModal();

} // namespace ADS::IDE

#endif // ADS_IDE_MODAL_SCAFFOLD_H
