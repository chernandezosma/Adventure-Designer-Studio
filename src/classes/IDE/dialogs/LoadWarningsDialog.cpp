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

#include <string>
#include <utility>

#include "imgui.h"

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
     * @param warnings Entities skipped by ProjectSerializer::load()
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

        ModalStyle style;
        style.initialSize = ImVec2(520.0f, 380.0f);
        style.minSize     = ImVec2(360.0f, 240.0f);
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

        const float footerHeight =
            ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        ImGui::BeginChild("##lw_list", ImVec2(0.0f, -footerHeight), true);
        for (const auto& warning : m_warnings) {
            const std::string line =
                warning.entityKind + " " + warning.identifier + ": " + warning.reason;
            ImGui::TextWrapped("%s", line.c_str());
            ImGui::Separator();
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button(tm->_t("DIALOG.OK").c_str(), ImVec2(90.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }

        endModal();
    }

} // namespace ADS::IDE
