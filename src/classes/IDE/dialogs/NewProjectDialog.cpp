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

#include "NewProjectDialog.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <filesystem>
#include <optional>

#include <nfd.hpp>
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include "spdlog/spdlog.h"

#include "ModalScaffold.h"
#include "Core/PathService.h"
#include "env/env.h"
#include "app.h"
#include "UI/Window.h"
#include "UI/NfdWindowHandle.h"

namespace ADS::IDE {

    namespace {
        /// Stable modal id ("###" => ImGui ignores the caption for identity).
        constexpr const char* kPopupId = "###ads_new_project";

        /**
         * @brief Map a Language catalog id to its index in `languageCatalog`.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param id A Language catalog id (src/constants/languages.h)
         * @return int The array index, or 0 when @p id is not in the catalog
         */
        int catalogIndexForId(std::uint8_t id)
        {
            const auto& cat = ADS::Constants::Languages::languageCatalog;
            for (std::size_t i = 0; i < cat.size(); ++i) {
                if (cat[i].id == id) {
                    return static_cast<int>(i);
                }
            }
            return 0;
        }
    } // namespace

    NewProjectDialog::NewProjectDialog()
    {
        resetBuffers();
    }

    /**
     * @brief Reset every input field to its default value.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    void NewProjectDialog::resetBuffers()
    {
        const NewProjectSpec defaults;

        std::snprintf(m_nameBuf, sizeof(m_nameBuf), "%s", defaults.name.c_str());
        std::snprintf(m_synopsisBuf, sizeof(m_synopsisBuf), "%s", defaults.synopsis.c_str());
        std::snprintf(m_authorNameBuf, sizeof(m_authorNameBuf), "%s", defaults.authorName.c_str());
        std::snprintf(m_authorEmailBuf, sizeof(m_authorEmailBuf), "%s", defaults.authorEmail.c_str());
        std::snprintf(m_versionBuf, sizeof(m_versionBuf), "%s", defaults.version.c_str());

        m_defaultLangIdx = catalogIndexForId(defaults.defaultLangId);
        m_supported.fill(false);
        for (std::uint8_t id : defaults.supportedLangIds) {
            m_supported[static_cast<std::size_t>(catalogIndexForId(id))] = true;
        }
        m_locationBuf[0] = '\0';
        m_locationOverridden = false;
        m_locationNeedsRecompute = true;
        m_error.clear();
    }

    /**
     * @brief Resolve the projects root, honouring the `.env` PROJECTS_DIR key.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::filesystem::path Core::PathService::projectsRoot() with the
     *         environment override applied
     */
    std::filesystem::path NewProjectDialog::resolveProjectsRoot()
    {
        std::string overrideDir;
        if (Environment* env = getEnvironment()) {
            overrideDir = env->getOrDefault("PROJECTS_DIR", "");
        }
        return Core::PathService::projectsRoot(overrideDir);
    }

    /**
     * @brief Arm the modal in Create mode, resetting every field to its
     *        default.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    void NewProjectDialog::open()
    {
        m_mode = Mode::Create;
        resetBuffers();
        m_pending = true;
    }

    /**
     * @brief Collect the current field values into a NewProjectSpec.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Fills `projectPath` from the (possibly user-overridden) Location field
     * and guarantees the default language is present in the supported set.
     *
     * @return NewProjectSpec The finished form data
     */
    NewProjectSpec NewProjectDialog::buildSpec()
    {
        const auto& cat = ADS::Constants::Languages::languageCatalog;

        NewProjectSpec spec;
        spec.name = m_nameBuf;
        spec.synopsis = m_synopsisBuf;
        spec.authorName = m_authorNameBuf;
        spec.authorEmail = m_authorEmailBuf;
        spec.version = m_versionBuf;
        spec.defaultLangId = cat[static_cast<std::size_t>(m_defaultLangIdx)].id;

        // m_locationBuf holds the override when set, and the name-derived path
        // otherwise (refreshed every frame in render()). Trim surrounding
        // whitespace and expand a leading "~" so a hand-edited value behaves
        // like the auto path.
        spec.projectPath = Core::PathService::expandUser(m_locationBuf);
        // Be forgiving if the user trimmed the extension while editing the
        // Location field — a project file is always ".ads".
        if (spec.projectPath.has_filename() &&
            spec.projectPath.extension() != ".ads") {
            spec.projectPath.replace_extension(".ads");
        }

        spec.supportedLangIds.clear();
        for (std::size_t i = 0; i < cat.size(); ++i) {
            if (m_supported[i]) {
                spec.supportedLangIds.push_back(cat[i].id);
            }
        }
        // The default language is always part of the supported set.
        if (std::find(spec.supportedLangIds.begin(), spec.supportedLangIds.end(),
                      spec.defaultLangId) == spec.supportedLangIds.end()) {
            spec.supportedLangIds.push_back(spec.defaultLangId);
        }
        return spec;
    }

    /**
     * @brief Render the modal for the current frame.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * A no-op frame unless the modal is armed or already open. Safe to call
     * unconditionally every frame.
     */
    void NewProjectDialog::render()
    {
        auto* tm = getTranslationManager();
        const auto& cat = ADS::Constants::Languages::languageCatalog;

        if (m_pending) {
            ImGui::OpenPopup(kPopupId);
            m_pending = false;
        }

        ModalStyle style;
        style.initialSize = ImVec2(560.0f, 640.0f);
        style.minSize     = ImVec2(440.0f, 320.0f);
        style.captionFont = getFontManager() ? getFontManager()->getFont("mediumFont") : nullptr;

        if (!beginModal(kPopupId, tm->_t("NEW_PROJECT.TITLE"), style)) {
            return;
        }

        // Escape aborts the dialog, same as the Cancel button.
        if (modalEscapeRequested()) {
            ImGui::CloseCurrentPopup();
            endModal();
            return;
        }

        // Width reserved on the right of every field row for the "browse"
        // button next to the Location input (button + spacing).
        constexpr float kBrowseButtonWidth = 26.0f;
        const float kLocationTrail = kBrowseButtonWidth + 8.0f;

        // Height reserved for the pinned footer (error slot + separator + button
        // row). Reserved unconditionally so the layout never jumps.
        const ImGuiStyle& imStyle = ImGui::GetStyle();
        const float footerHeight = imStyle.ItemSpacing.y
            + ImGui::GetTextLineHeightWithSpacing() * 2.0f  // error slot (2 lines, wrapped)
            + imStyle.ItemSpacing.y
            + ImGui::GetFrameHeightWithSpacing();     // separator + button row

        // Widest label decides the column so translated strings of any length
        // clear the input instead of being clipped behind it.
        const std::string labelKeys[] = {
            tm->_t("NEW_PROJECT.FIELD_NAME"),         tm->_t("NEW_PROJECT.FIELD_SYNOPSIS"),
            tm->_t("NEW_PROJECT.FIELD_AUTHOR_NAME"),  tm->_t("NEW_PROJECT.FIELD_AUTHOR_EMAIL"),
            tm->_t("NEW_PROJECT.FIELD_VERSION"),      tm->_t("NEW_PROJECT.FIELD_LOCATION"),
            tm->_t("NEW_PROJECT.FIELD_DEFAULT_LANG"),
        };
        float labelWidth = 0.0f;
        for (const std::string& s : labelKeys) {
            labelWidth = std::max(labelWidth, ImGui::CalcTextSize(s.c_str()).x);
        }
        labelWidth += ImGui::GetStyle().ItemInnerSpacing.x * 2.0f + 8.0f;

        // Draw a left-aligned label, then place the next widget to its right.
        // ImGui puts a widget's own label on its *right*, so every field uses a
        // hidden "##id" label and this helper for the visible one.
        const auto rowLabel = [&](const std::string& text) {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(text.c_str());
            ImGui::SameLine(labelWidth);
            ImGui::SetNextItemWidth(-FLT_MIN); // fill to the right edge — grows with the dialog
        };

        rowLabel(tm->_t("NEW_PROJECT.FIELD_NAME"));
        if (ImGui::InputText("##np_name", m_nameBuf, sizeof(m_nameBuf))) {
            m_locationNeedsRecompute = true; // refresh the derived Location once
        }

        rowLabel(tm->_t("NEW_PROJECT.FIELD_SYNOPSIS"));
        ImGui::InputTextMultiline("##np_synopsis", m_synopsisBuf, sizeof(m_synopsisBuf),
                                  ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3.0f));

        rowLabel(tm->_t("NEW_PROJECT.FIELD_AUTHOR_NAME"));
        ImGui::InputText("##np_author_name", m_authorNameBuf, sizeof(m_authorNameBuf));

        rowLabel(tm->_t("NEW_PROJECT.FIELD_AUTHOR_EMAIL"));
        ImGui::InputText("##np_author_email", m_authorEmailBuf, sizeof(m_authorEmailBuf));

        rowLabel(tm->_t("NEW_PROJECT.FIELD_VERSION"));
        ImGui::InputText("##np_version", m_versionBuf, sizeof(m_versionBuf));

        // Where the project file will be saved. Tracks the project name until the
        // user types, pastes or browses an explicit path (then left alone).
        // Recomputed only when the name changed (or on first open), not every
        // frame — PathService logs each resolve.
        if (!m_locationOverridden && m_locationNeedsRecompute) {
            const std::string computed =
                Core::PathService::projectFile(resolveProjectsRoot(), m_nameBuf).string();
            std::snprintf(m_locationBuf, sizeof(m_locationBuf), "%s", computed.c_str());
            m_locationNeedsRecompute = false;
        }
        rowLabel(tm->_t("NEW_PROJECT.FIELD_LOCATION"));
        ImGui::SetNextItemWidth(-kLocationTrail); // fill, leaving room for the browse button
        if (ImGui::InputText("##np_location", m_locationBuf, sizeof(m_locationBuf))) {
            m_locationOverridden = true; // any manual edit (incl. paste) is an override
        }
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button(ICON_FA_FOLDER_OPEN "##np_location_browse",
                          ImVec2(kBrowseButtonWidth, 0.0f))) {
            m_pendingPathDialog = true; // NFD runs deferred in processPendingDialogs()
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tm->_t("NEW_PROJECT.LOCATION_BROWSE_TIP").c_str());
        }

        ImGui::Spacing();

        // Default (game target) language — the full catalog is correct here:
        // these are compile targets, a different axis from the IDE UI language.
        rowLabel(tm->_t("NEW_PROJECT.FIELD_DEFAULT_LANG"));
        if (ImGui::BeginCombo("##np_default_lang",
                              std::string(cat[static_cast<std::size_t>(m_defaultLangIdx)].name).c_str())) {
            for (std::size_t i = 0; i < cat.size(); ++i) {
                const bool selected = (static_cast<int>(i) == m_defaultLangIdx);
                if (ImGui::Selectable(std::string(cat[i].name).c_str(), selected)) {
                    m_defaultLangIdx = static_cast<int>(i);
                    m_supported[i] = true; // default is always supported
                }
                if (selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TextUnformatted(tm->_t("NEW_PROJECT.FIELD_SUPPORTED_LANGS").c_str());
        // The languages list is the only scrolling region: it absorbs the
        // window's vertical slack so the footer below stays pinned. Full width
        // so it grows with the dialog.
        ImGui::BeginChild("##ads_new_project_langs",
                          ImVec2(0.0f, -footerHeight), true);
        for (std::size_t i = 0; i < cat.size(); ++i) {
            bool checked = m_supported[i];
            const bool isDefault = (static_cast<int>(i) == m_defaultLangIdx);

            ImGui::BeginDisabled(isDefault); // can't unselect the default language
            if (ImGui::Checkbox(std::string(cat[i].name).c_str(), &checked)) {
                m_supported[i] = checked;
            }
            ImGui::EndDisabled();
        }
        ImGui::EndChild(); // ##ads_new_project_langs

        // Error slot (kept present even when empty so the button row never shifts).
        if (m_error.empty()) {
            ImGui::NewLine();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.90f, 0.35f, 0.35f, 1.0f));
            ImGui::PushTextWrapPos(0.0f); // wrap at the window's right edge
            ImGui::TextUnformatted(m_error.c_str());
            ImGui::PopTextWrapPos();
            ImGui::PopStyleColor();
        }
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button(tm->_t("NEW_PROJECT.BTN_CREATE").c_str(), ImVec2(110, 0))) {
            const NewProjectSpec spec = buildSpec();
            if (spec.name.empty()) {
                m_error = tm->_t("NEW_PROJECT.ERR_NAME_REQUIRED");
            } else if (!spec.projectPath.has_filename()) {
                m_error = tm->_t("NEW_PROJECT.ERR_LOCATION_INVALID");
            } else if (!Core::PathService::isPathLengthValid(spec.projectPath)) {
                m_error = tm->_t("NEW_PROJECT.ERR_LOCATION_TOO_LONG");
            } else if (!m_locationOverridden &&
                       std::filesystem::exists(spec.projectPath)) {
                // Guard the auto path against clobbering an existing project.
                // An explicit path the user typed or browsed is trusted (the
                // OS Save dialog already prompts on overwrite).
                m_error = tm->_t("NEW_PROJECT.ERR_FOLDER_EXISTS");
            } else if (onCreate) {
                // onCreate writes the .ads file. Empty result => success, close;
                // otherwise keep the modal open and show why.
                const std::string failure = onCreate(spec);
                if (failure.empty()) {
                    ImGui::CloseCurrentPopup();
                } else {
                    m_error = tm->_t("NEW_PROJECT.ERR_CREATE_FAILED") + " " + failure;
                }
            } else {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(tm->_t("NEW_PROJECT.BTN_CANCEL").c_str(), ImVec2(110, 0))) {
            ImGui::CloseCurrentPopup();
        }

        endModal();
    }

    /**
     * @brief Service the deferred native "choose project file" dialog.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Must be called once per frame from IDERenderer::processPendingDialogs()
     * — i.e. AFTER SDL_RenderPresent — so the blocking NFD call never runs
     * mid-render (the gray-window artifact). Starts the picker when the
     * Location browse button was clicked and applies its result.
     */
    void NewProjectDialog::processPendingDialogs()
    {
        std::optional<std::string> result;
        if (m_pathDialog.poll(result)) {
            if (result) {
                std::snprintf(m_locationBuf, sizeof(m_locationBuf), "%s", result->c_str());
                m_locationOverridden = true;
            }
        }

        if (!m_pendingPathDialog || m_pathDialog.isRunning()) {
            return;
        }
        m_pendingPathDialog = false;

        // Seed the picker with the path currently shown in the Location field.
        const std::filesystem::path seed(m_locationBuf);
        std::string folder = seed.has_parent_path() ? seed.parent_path().string() : std::string{};
        std::string name   = seed.has_filename() ? seed.filename().string() : std::string{"project.ads"};

        nfdwindowhandle_t parent = ADS::UI::getNfdParentWindowHandle(
            Core::App::getMainWindow() ? Core::App::getMainWindow()->getWindow() : nullptr);

        m_pathDialog.start([parent, folder, name]() -> std::optional<std::string> {
            nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };
            NFD::Guard guard;
            NFD::UniquePath outPath;
            const nfdresult_t r = NFD::SaveDialog(
                outPath, filters, 1,
                folder.empty() ? nullptr : folder.c_str(),
                name.c_str(), parent);

            if (r == NFD_OKAY) return std::string(outPath.get());
            if (r == NFD_ERROR) spdlog::error("NewProjectDialog: NFD error — {}", NFD::GetError());
            return std::nullopt;
        });
    }

} // namespace ADS::IDE
