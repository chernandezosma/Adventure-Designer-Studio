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

#include "LanguagesDialog.h"

#include <algorithm>
#include <string>
#include <vector>

#include "imgui.h"

#include "ModalScaffold.h"

namespace ADS::IDE {

    namespace {
        /// Stable modal id ("###" => the caption does not affect identity).
        constexpr const char* kPopupId = "###ads_project_languages";
    } // namespace

    /**
     * @brief Arm the modal, seeded from @p current.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param current The project's present language set
     */
    void LanguagesDialog::open(const Data::GameLanguages& current)
    {
        const auto& cat = ADS::Constants::Languages::languageCatalog;

        m_defaultId = current.defaultId;
        m_checked.fill(false);
        for (std::size_t i = 0; i < cat.size(); ++i) {
            for (const std::uint8_t id : current.supportedIds) {
                if (cat[i].id == id) {
                    m_checked[i] = true;
                    break;
                }
            }
            if (cat[i].id == m_defaultId) {
                m_checked[i] = true; // default is always in the set
            }
        }
        m_pending = true;
    }

    /**
     * @brief Render the modal for the current frame.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * A no-op frame unless the modal is armed or already open.
     */
    void LanguagesDialog::render()
    {
        auto* tm = getTranslationManager();
        const auto& cat = ADS::Constants::Languages::languageCatalog;

        if (m_pending) {
            ImGui::OpenPopup(kPopupId);
            m_pending = false;
        }

        ModalStyle style;
        style.initialSize = ImVec2(440.0f, 520.0f);
        style.minSize     = ImVec2(360.0f, 320.0f);
        style.captionFont = getFontManager() ? getFontManager()->getFont("mediumFont") : nullptr;

        if (!beginModal(kPopupId, tm->_t("LANGUAGES_DIALOG.TITLE"), style)) {
            return;
        }

        // Escape aborts the dialog, same as the Cancel button.
        if (modalEscapeRequested()) {
            ImGui::CloseCurrentPopup();
            endModal();
            return;
        }

        ImGui::TextWrapped("%s", tm->_t("LANGUAGES_DIALOG.HINT").c_str());
        ImGui::Spacing();

        const float footerHeight =
            ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        ImGui::BeginChild("##pl_list", ImVec2(0.0f, -footerHeight), true);
        for (std::size_t i = 0; i < cat.size(); ++i) {
            const bool isDefault = (cat[i].id == m_defaultId);
            bool checked = m_checked[i];

            ImGui::BeginDisabled(isDefault); // the default language cannot be removed
            if (ImGui::Checkbox(std::string(cat[i].name).c_str(), &checked)) {
                m_checked[i] = checked;
            }
            ImGui::EndDisabled();
        }
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button(tm->_t("DIALOG.OK").c_str(), ImVec2(90.0f, 0.0f))) {
            Data::GameLanguages result;
            result.defaultId = m_defaultId;
            result.supportedIds.clear();
            for (std::size_t i = 0; i < cat.size(); ++i) {
                if (m_checked[i]) {
                    result.supportedIds.push_back(cat[i].id);
                }
            }
            // Guarantee the default is present even if the array somehow missed it.
            if (std::find(result.supportedIds.begin(), result.supportedIds.end(),
                          m_defaultId) == result.supportedIds.end()) {
                result.supportedIds.push_back(m_defaultId);
            }
            if (onApply) {
                onApply(result);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(tm->_t("DIALOG.CANCEL").c_str(), ImVec2(90.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }

        endModal();
    }

} // namespace ADS::IDE
