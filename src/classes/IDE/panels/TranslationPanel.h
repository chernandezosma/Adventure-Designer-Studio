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

#pragma once

/**
 * @file TranslationPanel.h
 * @brief Dockable panel for authoring the game's per-language translatable strings
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <array>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "BasePanel.h"
#include "Core/Project.h"
#include "Core/TranslationCatalog.h"
#include "IDE/dialogs/LanguagesDialog.h"

namespace ADS::IDE::Panels {

    /**
     * @brief Two-column translation editor: string list (left) + one text area
     *        per project language (right).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Backed by Core::TranslationCatalog over the current Core::Project; the
     * strings persist to the sibling `<project>.trn` file on project save.
     * Toggled from View ▸ Translations and the toolbar; hidden by default.
     */
    class TranslationPanel : public BasePanel {
    public:
        /**
         * @brief Construct the panel (hidden until toggled).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        TranslationPanel();

        /**
         * @brief Point the panel at the active project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param project The project (non-owning), or nullptr
         */
        void setProject(Core::Project* project);

        /**
         * @brief Render the panel for the current frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * No-op while hidden. Safe to call every frame.
         */
        void render() override;

        /// Invoked after any translation edit, so the owner can mark the project dirty.
        std::function<void()> onEdited;

    private:
        /// Max project languages a single string's editor will show.
        static constexpr int kMaxLangs = 12;
        /// Per-language text-area buffer size.
        static constexpr int kBufSize = 4096;
        /// Character cap for a translatable description string, matching the
        /// inspector's PropertyConstraints::translatableText limit.
        static constexpr std::size_t kMaxDescriptionLen = 512;

        /**
         * @brief Reload the right-pane buffers for @p m_selectedId.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        void reloadBuffers();

        /**
         * @brief i18n key for a translation group's header.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param group The group
         * @return const char* A `TRANSLATIONS.GROUP_*` key
         */
        static const char* groupKey(Core::TranslationGroup group);

        /**
         * @brief i18n key for a translation field's row label.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param field The field
         * @return const char* A `TRANSLATIONS.FIELD_*` key
         */
        static const char* fieldKey(Core::TranslationField field);

        Core::Project* m_project = nullptr; ///< Active project (non-owning), set via setProject().

        std::vector<Core::TranslationEntry> m_entries;  ///< rebuilt each frame from the project
        std::string m_selectedId;                       ///< id of the row shown on the right
        std::string m_loadedForId;                      ///< which id the buffers currently hold
        bool m_wasVisible = false;                       ///< previous frame's visibility, for the open edge

        // ---- on-demand rescan of every translatable value --------------------
        bool        m_scanQueued = false;  ///< a rescan was asked for (panel opened, or toolbar button)
        bool        m_scanActive = false;  ///< a rescan is running across frames
        std::size_t m_scanIndex  = 0;      ///< next entry the running scan will touch
        std::size_t m_scanTotal  = 0;      ///< entry count captured when the scan started

        int m_langCount = 0;                             ///< project languages currently shown
        std::array<std::string, kMaxLangs> m_langCode{}; ///< code per right-pane slot
        std::array<bool, kMaxLangs> m_langIsDefault{};   ///< default-language flag per slot
        std::array<std::array<char, kBufSize>, kMaxLangs> m_langBuf{}; ///< editable text per slot

        LanguagesDialog m_langDialog; ///< "manage project languages" modal (toolbar button)
    };

} // namespace ADS::IDE::Panels
