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

#ifndef ADS_IDE_NEW_PROJECT_DIALOG_H
#define ADS_IDE_NEW_PROJECT_DIALOG_H

/**
 * @file NewProjectDialog.h
 * @brief Modal that collects the settings for a brand-new project
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <array>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "../IDEBase.h"
#include "Core/PathService.h"
#include "UI/AsyncFileDialog.h"
#include "languages.h"

namespace ADS::IDE {

    /**
     * @brief Plain result of the New Project dialog.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The consumer (IDERenderer) maps `name` onto `game.title` (which is the
     * project name — see Core::Project::getName()) and the rest onto
     * Data::GameData, then sets the project's file path to `projectPath`.
     * `defaultLangId` / `supportedLangIds` are Language catalog ids
     * (src/constants/languages.h); the dialog guarantees
     * `defaultLangId ∈ supportedLangIds`.
     */
    struct NewProjectSpec {
        std::string name;         ///< Project name == game.title (required)
        std::string synopsis;     ///< game.synopsis (optional)
        std::string authorName;   ///< game.author.name (optional)
        std::string authorEmail;  ///< game.author.email (optional)
        std::string version;      ///< game.version.version
        std::uint8_t defaultLangId = 21;              ///< game.languages.default (21 = en_US)
        std::vector<std::uint8_t> supportedLangIds{21}; ///< game.languages.supported
        std::filesystem::path projectPath;           ///< Resolved <root>/<slug>/<slug>.ads target
    };

    /**
     * @brief Pure-ImGui modal for creating a new project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * open() arms the modal; render() must be called every frame from within
     * an active ImGui window, outside any BeginMenu/EndMenu scope. On "Create"
     * the collected NewProjectSpec is handed to the onCreate callback and the
     * popup closes; "Cancel" closes with no side effect (the caller keeps its
     * current project). No native OS dialogs are involved, so this never
     * touches the deferred processPendingDialogs() chain.
     *
     * The Mode enum is present so a later "Project Settings" editor can reuse
     * the same form via openForEdit()/onApply without a redesign.
     */
    class NewProjectDialog : public IDEBase {
    public:
        /// Whether the form creates a new project or edits an existing one.
        enum class Mode { Create, Edit };

        /**
         * @brief Construct the dialog with every field at its default value.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         */
        NewProjectDialog();

        /// Invoked with the finished spec when the user clicks Create. Returns
        /// an empty string on success (the modal then closes) or a failure
        /// reason to show inline (the modal stays open). The consumer
        /// (IDERenderer) writes the project's `.ads` file here, so a bad
        /// Location surfaces before the dialog is dismissed.
        std::function<std::string(const NewProjectSpec&)> onCreate;

        /**
         * @brief Arm the modal in Create mode, resetting every field to its
         *        default.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        void open();

        /**
         * @brief Render the modal for the current frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * A no-op frame unless the modal is armed or already open. Safe to call
         * unconditionally every frame.
         */
        void render();

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
        void processPendingDialogs();

        /**
         * @brief True while the native "choose project file" picker is open.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return bool true if the background NFD save dialog is running
         */
        [[nodiscard]] bool isFileDialogInProgress() const { return m_pathDialog.isRunning(); }

    private:
        /// Number of entries in the language catalog (fixes the checkbox array size).
        static constexpr std::size_t kCatalogSize =
            ADS::Constants::Languages::languageCatalog.size();

        /**
         * @brief Reset every input field to its default value.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        void resetBuffers();

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
        [[nodiscard]] NewProjectSpec buildSpec();

        /**
         * @brief Resolve the projects root, honouring the `.env` PROJECTS_DIR key.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return std::filesystem::path Core::PathService::projectsRoot() with the
         *         environment override applied
         */
        [[nodiscard]] std::filesystem::path resolveProjectsRoot();

        Mode m_mode = Mode::Create;
        bool m_pending = false; ///< set by open(), consumed on the next render()

        char m_nameBuf[128]{};
        char m_synopsisBuf[1024]{};
        char m_authorNameBuf[128]{};
        char m_authorEmailBuf[256]{};
        char m_versionBuf[16]{};
        char m_locationBuf[Core::PathService::kPathBufferCapacity]{}; ///< editable
            ///< project-file path (paste/type/browse). Sized to this platform's
            ///< worst-case path ceiling (Core::PathService::kPathBufferCapacity)
            ///< so a legitimately valid path is never silently truncated before
            ///< Core::PathService::isPathLengthValid() gets to check it in
            ///< render() — buffer capacity and the enforced OS limit are two
            ///< different things, but must not fall out of sync.

        int m_defaultLangIdx = 0;                    ///< index into languageCatalog
        std::array<bool, kCatalogSize> m_supported{}; ///< checkbox state per catalog entry

        std::string m_error; ///< inline validation message, empty when valid

        // --- Location field state ---
        ADS::UI::AsyncFileDialog m_pathDialog;    ///< threaded native save picker (browse button)
        bool m_pendingPathDialog  = false;        ///< browse button clicked, picker not yet started
        bool m_locationOverridden = false;        ///< user typed/pasted/browsed an explicit path;
                                                 ///< stop tracking the name, skip the folder guard
        bool m_locationNeedsRecompute = true;    ///< the name-derived Location is stale (name
                                                 ///< changed / dialog just opened) — refresh once
    };

} // namespace ADS::IDE

#endif // ADS_IDE_NEW_PROJECT_DIALOG_H
