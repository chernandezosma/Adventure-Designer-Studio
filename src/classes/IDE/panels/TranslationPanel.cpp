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

#include "TranslationPanel.h"

#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <cstdio>

#include "imgui.h"
#include "IconsFontAwesome4.h"

#include "IDE/DesignTokens.h"
#include "IDE/dialogs/ExpandableTextInput.h"
#include "languages.h"

namespace ADS::IDE::Panels {

    using ADS::Core::TranslationCatalog;
    using ADS::Core::TranslationEntry;
    using ADS::Core::TranslationField;
    using ADS::Core::TranslationGroup;

    TranslationPanel::TranslationPanel()
        : BasePanel("Translations")
    {
        m_titleKey = "TRANSLATIONS.WINDOW_TITLE";
        m_isVisible = false; // opened on demand from View ▸ Translations

        // Apply an edited language set to the project and force a right-pane
        // rebuild on the next frame.
        m_langDialog.onApply = [this](const Data::GameLanguages& gl) {
            if (m_project == nullptr) {
                return;
            }
            m_project->getGameData().setLanguages(gl);
            m_loadedForId.clear();
            m_langCount = 0;
            if (onEdited) {
                onEdited();
            }
        };
    }

    /**
     * @brief Point the panel at the active project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param project The project (non-owning), or nullptr
     */
    void TranslationPanel::setProject(Core::Project* project)
    {
        m_project = project;
        m_selectedId.clear();
        m_loadedForId.clear();
        m_langCount = 0;
        m_scanQueued = true;   // rescan values for the newly-attached project
        m_scanActive = false;
    }

    /**
     * @brief i18n key for a translation group's header.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param group The group
     * @return const char* A `TRANSLATIONS.GROUP_*` key
     */
    const char* TranslationPanel::groupKey(TranslationGroup group)
    {
        switch (group) {
            case TranslationGroup::Scenes:     return "TRANSLATIONS.GROUP_SCENES";
            case TranslationGroup::States:     return "TRANSLATIONS.GROUP_STATES";
            case TranslationGroup::Characters: return "TRANSLATIONS.GROUP_CHARACTERS";
            case TranslationGroup::Items:      return "TRANSLATIONS.GROUP_ITEMS";
        }
        return "TRANSLATIONS.GROUP_SCENES";
    }

    /**
     * @brief i18n key for a translation field's row label.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param field The field
     * @return const char* A `TRANSLATIONS.FIELD_*` key
     */
    const char* TranslationPanel::fieldKey(TranslationField field)
    {
        switch (field) {
            case TranslationField::Name:       return "TRANSLATIONS.FIELD_NAME";
            case TranslationField::DescNormal: return "TRANSLATIONS.FIELD_DESC_NORMAL";
            case TranslationField::DescLong:   return "TRANSLATIONS.FIELD_DESC_LONG";
            case TranslationField::DescOdor:   return "TRANSLATIONS.FIELD_DESC_ODOR";
            case TranslationField::DescSound:  return "TRANSLATIONS.FIELD_DESC_SOUND";
        }
        return "TRANSLATIONS.FIELD_NAME";
    }

    /**
     * @brief Reload the right-pane buffers for @p m_selectedId.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    void TranslationPanel::reloadBuffers()
    {
        for (auto& buf : m_langBuf) {
            buf[0] = '\0';
        }
        m_langCode.fill(std::string{});
        m_langIsDefault.fill(false);
        m_langCount = 0;

        if (m_project == nullptr || m_selectedId.empty()) {
            m_loadedForId = m_selectedId;
            return;
        }

        const auto& langs = m_project->getGameData().getLanguages();
        for (const std::uint8_t id : langs.supportedIds) {
            if (m_langCount >= kMaxLangs) {
                break;
            }
            const std::string code = ADS::Constants::Languages::getLanguageCodeById(id);
            if (code.empty()) {
                continue;
            }
            const std::string text = TranslationCatalog::get(*m_project, m_selectedId, code);
            std::snprintf(m_langBuf[m_langCount].data(), kBufSize, "%s", text.c_str());
            m_langCode[m_langCount] = code;
            m_langIsDefault[m_langCount] = (id == langs.defaultId);
            ++m_langCount;
        }
        m_loadedForId = m_selectedId;
    }

    /**
     * @brief Render the panel for the current frame.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * No-op while hidden. Safe to call every frame.
     */
    void TranslationPanel::render()
    {
        // Every time the panel is (re)opened, queue a full rescan of the
        // project's translatable values so anything edited elsewhere — a name
        // or description changed in the Inspector — is picked up.
        const bool becameVisible = m_isVisible && !m_wasVisible;
        m_wasVisible = m_isVisible;

        if (!m_isVisible) {
            return;
        }

        if (becameVisible) {
            m_scanQueued = true;
        }

        using namespace ADS::IDE::Colors;

        // Floating by default (not in the LayoutManager dock map); give it a
        // sensible first-run size and centre so it doesn't open behind the
        // dockspace as a 0-size sliver.
        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x * 0.6f, vp->Size.y * 0.6f),
                                 ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, BG1);
        const bool windowOpen = beginWindow(&m_isVisible);
        ImGui::PopStyleColor();
        if (!windowOpen) {
            endWindow();
            return;
        }

        const auto* tm = getTranslationsManager();

        if (m_project == nullptr) {
            ImGui::TextDisabled("%s", tm->_t("TRANSLATIONS.NO_PROJECT").c_str());
            endWindow();
            return;
        }

        // Keep the tree structurally current every idle frame (cheap); the
        // running scan freezes it so its index stays valid.
        if (!m_scanActive) {
            m_entries = TranslationCatalog::enumerate(*m_project);
        }

        // ---- toolbar -----------------------------------------------------
        if (ImGui::Button(ICON_FA_REFRESH "##trn_rescan")) {
            m_scanQueued = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tm->_t("TRANSLATIONS.RESCAN_TIP").c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_LANGUAGE "##trn_langs")) {
            m_langDialog.open(m_project->getGameData().getLanguages());
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tm->_t("TRANSLATIONS.MANAGE_LANGS_TIP").c_str());
        }
        m_langDialog.render();
        ImGui::Separator();
        ImGui::Spacing();

        // ---- on-demand rescan of every translatable value --------------------
        if (m_scanQueued && !m_scanActive) {
            m_scanQueued = false;
            m_scanActive = true;
            m_scanIndex  = 0;
            m_scanTotal  = m_entries.size();
        }

        if (m_scanActive) {
            const auto& langs = m_project->getGameData().getLanguages();
            constexpr std::size_t kChunk = 64;
            const std::size_t end = std::min(m_scanIndex + kChunk, m_scanTotal);
            for (; m_scanIndex < end; ++m_scanIndex) {
                const std::string& id = m_entries[m_scanIndex].id;
                for (const std::uint8_t lid : langs.supportedIds) {
                    const std::string code =
                        ADS::Constants::Languages::getLanguageCodeById(lid);
                    if (!code.empty()) {
                        (void) TranslationCatalog::get(*m_project, id, code);
                    }
                }
            }
            if (m_scanIndex >= m_scanTotal) {
                m_scanActive = false;
                m_loadedForId.clear(); // reload the editor buffers with fresh values
            }

            const float frac =
                m_scanTotal ? static_cast<float>(m_scanIndex) /
                                  static_cast<float>(m_scanTotal)
                            : 1.0f;
            char overlay[48];
            std::snprintf(overlay, sizeof(overlay), "%zu / %zu",
                          m_scanIndex, m_scanTotal);
            ImGui::Spacing();
            ImGui::TextUnformatted(tm->_t("TRANSLATIONS.SCANNING").c_str());
            ImGui::Spacing();
            ImGui::ProgressBar(frac, ImVec2(-FLT_MIN, 0.0f), overlay);
            endWindow();
            return;
        }

        if (m_entries.empty()) {
            ImGui::TextDisabled("%s", tm->_t("TRANSLATIONS.EMPTY").c_str());
            endWindow();
            return;
        }

        const float leftWidth = ImGui::GetContentRegionAvail().x * 0.25f;

        // ---- left: grouped string list -------------------------------------
        ImGui::BeginChild("##trn_list", ImVec2(leftWidth, 0.0f), true);
        {
            TranslationGroup currentGroup = m_entries.front().group;
            bool groupOpen = false;
            std::string currentEntity;
            bool entityOpen = false;
            bool descOpen = false; // "Descriptions" sub-node under the current entity
            bool first = true;

            const auto closeDescNode = [&]() {
                if (descOpen) { ImGui::TreePop(); descOpen = false; }
            };

            for (std::size_t i = 0; i < m_entries.size(); ++i) {
                const TranslationEntry& e = m_entries[i];

                if (first || e.group != currentGroup) {
                    if (!first) {
                        closeDescNode();
                        if (entityOpen) { ImGui::TreePop(); entityOpen = false; }
                    }
                    currentGroup = e.group;
                    currentEntity.clear();
                    groupOpen = ImGui::CollapsingHeader(
                        tm->_t(groupKey(e.group)).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                    first = false;
                }
                if (!groupOpen) {
                    continue;
                }

                if (e.entityLabel != currentEntity) {
                    closeDescNode();
                    if (entityOpen) { ImGui::TreePop(); entityOpen = false; }
                    currentEntity = e.entityLabel;
                    ImGui::PushID(static_cast<int>(i));
                    entityOpen = ImGui::TreeNodeEx(
                        currentEntity.empty() ? "(unnamed)" : currentEntity.c_str(),
                        ImGuiTreeNodeFlags_SpanAvailWidth);
                    ImGui::PopID();
                }
                if (!entityOpen) {
                    continue;
                }

                const bool isDesc = (e.field != TranslationField::Name);

                // Open the "Descriptions" sub-node just before its first slot.
                if (isDesc && e.field == TranslationField::DescNormal) {
                    ImGui::PushID(static_cast<int>(i));
                    descOpen = ImGui::TreeNodeEx(
                        tm->_t("CATEGORY.DESCRIPTIONS").c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);
                    ImGui::PopID();
                }

                if (!isDesc || descOpen) {
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::Selectable(tm->_t(fieldKey(e.field)).c_str(), e.id == m_selectedId)) {
                        m_selectedId = e.id;
                    }
                    ImGui::PopID();
                }

                // Close it after the last slot.
                if (isDesc && e.field == TranslationField::DescSound) {
                    closeDescNode();
                }
            }
            closeDescNode();
            if (entityOpen) {
                ImGui::TreePop();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // ---- right: one text area per project language --------------------
        ImGui::BeginChild("##trn_editor", ImVec2(0.0f, 0.0f), true);
        {
            if (m_selectedId.empty()) {
                ImGui::TextDisabled("%s", tm->_t("TRANSLATIONS.SELECT_HINT").c_str());
            } else {
                if (m_selectedId != m_loadedForId) {
                    reloadBuffers();
                } else if (!ImGui::IsAnyItemActive()) {
                    // Keep the buffers live against edits made elsewhere while
                    // the panel stays open on the same row. Safe to overwrite:
                    // every edit here is flushed to the catalog immediately, so
                    // the buffers never hold anything unsaved.
                    for (int i = 0; i < m_langCount; ++i) {
                        const std::string current =
                            TranslationCatalog::get(*m_project, m_selectedId, m_langCode[i]);
                        if (current != m_langBuf[i].data()) {
                            reloadBuffers();
                            break;
                        }
                    }
                }

                // Heading: which string is being edited.
                const auto sel = std::find_if(
                    m_entries.begin(), m_entries.end(),
                    [&](const TranslationEntry& e) { return e.id == m_selectedId; });
                if (sel != m_entries.end()) {
                    ImGui::TextUnformatted(sel->entityLabel.c_str());
                    ImGui::SameLine();
                    if (sel->field == TranslationField::Name) {
                        ImGui::TextDisabled("/ %s", tm->_t(fieldKey(sel->field)).c_str());
                    } else {
                        ImGui::TextDisabled("/ %s / %s",
                                            tm->_t("CATEGORY.DESCRIPTIONS").c_str(),
                                            tm->_t(fieldKey(sel->field)).c_str());
                    }
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                }

                const std::string fieldName = tm->_t(fieldKey(sel != m_entries.end()
                                                                  ? sel->field
                                                                  : TranslationField::Name));
                for (int i = 0; i < m_langCount; ++i) {
                    std::string langName = ADS::Constants::Languages::getLanguageName(m_langCode[i]);
                    if (langName.empty()) {
                        langName = m_langCode[i];
                    }
                    std::string label = langName;
                    if (m_langIsDefault[i]) {
                        label += "  ";
                        label += tm->_t("TRANSLATIONS.DEFAULT_LANG_TAG");
                    }
                    ImGui::TextUnformatted(label.c_str());

                    // Inline field + "…" expand dialog, matching the inspector's
                    // description editor.
                    const std::string dialogTitle =
                        (sel != m_entries.end() ? sel->entityLabel : std::string{})
                        + "  ·  " + fieldName + "  ·  " + langName;

                    ImGui::PushID(i);
                    if (ADS::IDE::expandableTextInput(
                            "##trn_val", m_langBuf[i].data(), kBufSize, dialogTitle,
                            tm->_t("DIALOG.OK").c_str(), tm->_t("DIALOG.CANCEL").c_str(),
                            kMaxDescriptionLen)) {
                        TranslationCatalog::set(*m_project, m_selectedId, m_langCode[i],
                                                std::string(m_langBuf[i].data()));
                        if (onEdited) {
                            onEdited();
                        }
                    }
                    ImGui::PopID();
                    ImGui::Spacing();
                }
            }
        }
        ImGui::EndChild();

        endWindow();
    }

} // namespace ADS::IDE::Panels
