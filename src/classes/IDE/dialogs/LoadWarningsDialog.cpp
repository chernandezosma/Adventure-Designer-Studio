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

#include "LoadWarningsDialog.h"

#include <algorithm>
#include <string>
#include <utility>

#include "imgui.h"

#include "../DesignTokens.h"
#include "ModalScaffold.h"

namespace ADS::IDE {

    namespace {
        /// Stable modal id ("###" => the caption does not affect identity).
        constexpr const char* kPopupId = "###ads_load_warnings";
    } // namespace

    /**
     * @brief Arm the modal with @p warnings.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param warnings Entities that were skipped or loaded with defaulted
     *        fields, from Core::LoadResult::warnings
     */
    void LoadWarningsDialog::open(std::vector<Core::LoadWarning> warnings)
    {
        m_warnings = std::move(warnings);
        m_pending = true;
    }

    /**
     * @brief Render the modal for the current frame.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * A no-op frame unless the modal is armed or already open.
     */
    void LoadWarningsDialog::render()
    {
        auto* tm = getTranslationManager();

        if (m_pending) {
            ImGui::OpenPopup(kPopupId);
            m_pending = false;
        }

        // Shared with the measurement pass below and the render loop further
        // down, so the two never drift apart.
        auto formatLine = [&](const Core::LoadWarning& warning) {
            const std::string status = warning.skipped
                ? tm->_t("LOAD_WARNINGS_DIALOG.SKIPPED")
                : tm->_t("LOAD_WARNINGS_DIALOG.DEFAULTED");
            return warning.entityKind + " " + warning.identifier + " (" + status + "): " + warning.reason;
        };

        // Sized to the longest warning line, capped at 75% of the
        // working-area width; centered by beginModal()'s own
        // SetNextWindowPos(Appearing) call.
        const ImVec2 workSize = ImGui::GetMainViewport()->WorkSize;
        float longestLineWidth = 0.0f;
        for (const auto& warning : m_warnings) {
            longestLineWidth = std::max(longestLineWidth, ImGui::CalcTextSize(formatLine(warning).c_str()).x);
        }
        const float widthChrome =
            ImGui::GetStyle().WindowPadding.x * 2.0f + ImGui::GetStyle().ScrollbarSize;
        const float dialogWidth = std::min(longestLineWidth + widthChrome, workSize.x * 0.75f);

        // Extra breathing room above and below the OK button, on top of its
        // own frame height, so it doesn't sit flush against the separators.
        constexpr float kButtonPadY = 12.0f;

        // Real footer height, measured once it's actually drawn below and
        // carried over to the next frame; seeded with a formula estimate so
        // the very first frame still reserves roughly the right amount of
        // room. Sizing the list child off this measured value — rather than
        // a guess — is what keeps the OK button pinned exactly at the
        // window's bottom edge, including while the user drags that edge to
        // resize (ModalScaffold disables the outer window's own scrollbar,
        // so this child is the dialog's only scrolling region).
        static float s_footerHeight = 0.0f;
        const float footerHeightEstimate =
            1.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f       // separator above the footer
            + 2.0f * kButtonPadY + ImGui::GetFrameHeightWithSpacing(); // OK button + padding
        const float footerHeight = (s_footerHeight > 0.0f) ? s_footerHeight : footerHeightEstimate;

        const float chromeHeight =
            ImGui::GetTextLineHeightWithSpacing()   // hint (single line in practice)
            + ImGui::GetStyle().ItemSpacing.y       // Spacing() after the hint
            + footerHeight
            + ImGui::GetStyle().WindowPadding.y * 2.0f;

        // One line + separator per warning.
        const float rowHeight =
            ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().ItemSpacing.y + 1.0f;
        const float listHeight = static_cast<float>(m_warnings.size()) * rowHeight;

        // Grown to fit, capped at 50% of the app's own height. But a
        // scrollbar for a sliver of overflow is worse than just growing a
        // little past the cap: if the extra content is under 10% of the
        // capped height, grow to fit instead of showing a barely-there
        // scroll range. Still user-resizable afterward since beginModal()
        // only applies this on first appear.
        const float maxDialogHeight = workSize.y * 0.50f;
        const float desiredHeight   = chromeHeight + listHeight;
        float dialogHeight = desiredHeight;
        if (desiredHeight > maxDialogHeight) {
            const float overflow = desiredHeight - maxDialogHeight;
            dialogHeight = (overflow < maxDialogHeight * 0.10f) ? desiredHeight : maxDialogHeight;
        }

        ModalStyle style;
        style.initialSize = ImVec2(dialogWidth, dialogHeight);
        style.minSize     = ImVec2(360.0f, 200.0f);
        style.captionFont = getFontManager() ? getFontManager()->getFont("mediumFont") : nullptr;

        if (!beginModal(kPopupId, tm->_t("LOAD_WARNINGS_DIALOG.TITLE"), style)) {
            return;
        }

        // Escape acknowledges the dialog, same as the OK button.
        if (modalEscapeRequested()) {
            ImGui::CloseCurrentPopup();
            endModal();
            return;
        }

        ImGui::TextWrapped("%s", tm->_t("LOAD_WARNINGS_DIALOG.HINT").c_str());
        ImGui::Spacing();

        // The only scrolling region in the dialog: filled to whatever space
        // is left once footerHeight is set aside below it, so it grows and
        // shrinks with the window instead of leaving the footer stranded.
        ImGui::BeginChild("##lw_list", ImVec2(0.0f, -footerHeight), true);
        for (const auto& warning : m_warnings) {
            // Skipped (bad/duplicate id) is a harder loss than a loaded entity
            // with defaulted fields — color them differently so the reader
            // can tell "missing entirely" from "loaded, but check these" apart
            // at a glance.
            const ImVec4& color = warning.skipped ? Colors::C_ERROR : Colors::C_WARN;
            const std::string line = formatLine(warning);
            ImGui::TextColored(color, "%s", line.c_str());
            ImGui::Separator();
        }
        ImGui::EndChild();

        const float footerTop = ImGui::GetCursorPosY();

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, kButtonPadY));

        // Right-align the single OK button — no Cancel to balance against.
        constexpr float kButtonWidth = 90.0f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - kButtonWidth);
        if (ImGui::Button(tm->_t("DIALOG.OK").c_str(), ImVec2(kButtonWidth, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, kButtonPadY));

        // Carried into next frame's footerHeight (see above) so the list
        // child's reserved gap always matches what the footer actually
        // takes up, regardless of font/style changes.
        s_footerHeight = ImGui::GetCursorPosY() - footerTop;

        endModal();
    }

} // namespace ADS::IDE
