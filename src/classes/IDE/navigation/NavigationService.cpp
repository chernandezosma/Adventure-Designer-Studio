/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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


#include "NavigationService.h"
#include "imgui.h"
#include "spdlog/spdlog.h"
#include <nfd.hpp>
#include <filesystem>
#include "UI/NfdWindowHandle.h"
#include "app.h"
#include "../dialogs/ModalScaffold.h"

namespace ADS::IDE {

    /**
     * @brief Register project-awareness callbacks
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Stores the two callbacks that let the service query whether a project is
     * open and request the creation of a new one. Both parameters are moved into
     * the corresponding member functions.
     *
     * @param hasProject   Predicate returning true when a project is active
     * @param onNewProject Callable invoked when a new project should be created
     */
    void NavigationService::setProjectCallbacks(
        std::function<bool()> hasProject,
        std::function<void()> onNewProject)
    {
        m_hasActiveProject = std::move(hasProject);
        m_onNewProject     = std::move(onNewProject);
    }

    /**
     * @brief Register file I/O callbacks used by the native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Stores the open and save path callbacks so that dialog results can be
     * forwarded to the application layer without a direct dependency on
     * IDERenderer or any persistence layer.
     *
     * @param onOpen Callable receiving the selected open path (called on NFD_OKAY)
     * @param onSave Callable receiving the selected save path (called on NFD_OKAY)
     */
    void NavigationService::setFileCallbacks(
        std::function<void(const std::string&)> onOpen,
        std::function<void(const std::string&)> onSave)
    {
        m_onOpenProject = std::move(onOpen);
        m_onSaveProject = std::move(onSave);
    }

    /**
     * @brief Register the provider for the project's current file path
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getPath Callable returning the saved path, or std::nullopt when
     *                the project has never been saved
     */
    void NavigationService::setProjectPathProvider(std::function<std::optional<std::string>()> getPath)
    {
        m_getProjectPath = std::move(getPath);
    }

    /**
     * @brief Register the provider for the native pickers' start folder
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getDir Callable returning the folder the Open / Save As dialogs
     *               should open in (the projects root), or an empty string
     *               to leave it to the OS
     */
    void NavigationService::setDefaultDirProvider(std::function<std::string()> getDir)
    {
        m_defaultDir = std::move(getDir);
    }

    /**
     * @brief Register the provider for the save picker's pre-filled path
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getPrefill Callable returning a full `.ads` path to seed the
     *                   Save / Save As dialog (folder + filename), or an
     *                   empty string for no hint
     */
    void NavigationService::setSavePrefillProvider(std::function<std::string()> getPrefill)
    {
        m_getSavePrefill = std::move(getPrefill);
    }

    /**
     * @brief Handle the File Save action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * If the project already has a file path, saves straight to it via the
     * onSave callback (no dialog). Otherwise behaves like Save As, scheduling
     * the deferred native save picker.
     */
    void NavigationService::fileSaveHandler()
    {
        spdlog::info("Call NavigationService::fileSaveHandler");

        std::optional<std::string> existing;
        if (m_getProjectPath) {
            existing = m_getProjectPath();
        }

        if (existing && !existing->empty()) {
            if (m_onSaveProject) m_onSaveProject(*existing);
        } else {
            // Never saved — fall back to Save As.
            m_pendingSaveDialog = true;
            m_pendingSaveAndNew = false;
        }
    }

    /**
     * @brief Handle the File Save As action — always shows the save picker
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    void NavigationService::fileSaveAsHandler()
    {
        spdlog::info("Call NavigationService::fileSaveAsHandler");
        m_pendingSaveDialog = true;
        m_pendingSaveAndNew = false;
    }

    /**
     * @brief Handle the File Open action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the File Open action triggered from menu items or toolbar buttons.
     * Currently logs the action to the console using spdlog for debugging and
     * verification purposes.
     *
     * Future implementation will:
     * - Open a native file dialog for file selection
     * - Validate the selected file
     * - Load and parse the file content
     * - Update the IDE state with the loaded file
     *
     * @note Currently contains placeholder implementation (logging only)
     * @see MenuBarRenderer::renderFileMenu()
     * @see ToolBarRenderer::renderFileButtons()
     */
    void NavigationService::fileOpenHandler()
    {
        spdlog::info("Call NavigationService::fileOpenHandler");
        // Defer the NFD call to processPendingDialogs() so it runs after
        // SDL_RenderPresent, preventing the gray-window artifact.
        m_pendingOpenDialog = true;
    }

    /**
     * @brief Handle the File New action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Checks whether a project is currently active. If none is open the new-project
     * callback is invoked immediately. If a project is already loaded a confirmation
     * modal is scheduled; it will be rendered on the next frame(s) by renderDialogs().
     *
     * @see setProjectCallbacks()
     * @see renderDialogs()
     * @see MenuBarRenderer::renderFileMenu()
     * @see ToolBarRenderer::renderFileButtons()
     */
    void NavigationService::fileNewHandler()
    {
        spdlog::info("Call NavigationService::fileNewHandler");

        if (m_hasActiveProject && m_hasActiveProject()) {
            // A project is open — ask the user before discarding it
            m_confirmNewDialogOpen = true;
        } else {
            // No project open — create immediately
            if (m_onNewProject) m_onNewProject();
        }
    }

    /**
     * @brief Render any pending modal dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Opens the "New project" confirmation popup when m_confirmNewDialogOpen is set,
     * then services the modal every frame until the user makes a choice:
     *
     * - **Save**    — placeholder (logs a warning, then creates the new project)
     * - **Discard** — discards the current project and creates a new one
     * - **Cancel**  — closes the popup without any change
     *
     * Must be called once per frame from within an active ImGui window context
     * (outside any BeginMenu / EndMenu scope).
     */
    void NavigationService::renderDialogs()
    {
        // Schedule the popup when fileNewHandler flagged it
        if (m_confirmNewDialogOpen) {
            ImGui::OpenPopup(NEW_PROJECT_POPUP_ID);
            m_confirmNewDialogOpen = false;
        }

        auto* tm = Core::App::getTranslationsManager();

        ADS::IDE::ModalStyle style;
        style.initialSize = ImVec2(420.0f, 0.0f);
        style.autoResize  = true;
        if (auto* fonts = Core::App::getFontManager()) {
            style.captionFont = fonts->getFont("mediumFont");
        }

        if (ADS::IDE::beginModal(NEW_PROJECT_POPUP_ID, tm->_t("DIALOG.CONFIRM_NEW_CAPTION"), style))
        {
            // Escape aborts, same as the Cancel button.
            if (ADS::IDE::modalEscapeRequested()) {
                spdlog::info("NavigationService: confirm-new — user pressed Escape");
                ImGui::CloseCurrentPopup();
                ADS::IDE::endModal();
                return;
            }

            ImGui::TextUnformatted(tm->_t("DIALOG.CONFIRM_NEW_TITLE").c_str());
            ImGui::TextUnformatted(tm->_t("DIALOG.CONFIRM_NEW_BODY").c_str());
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button(tm->_t("DIALOG.CONFIRM_NEW_SAVE").c_str(), ImVec2(90, 0))) {
                spdlog::info("NavigationService: confirm-new — user chose Save");
                // Defer the NFD save dialog to processPendingDialogs().
                // The confirm modal closes now so the compositor gets a clean
                // frame before the blocking call.
                m_pendingSaveDialog  = true;
                m_pendingSaveAndNew  = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button(tm->_t("DIALOG.CONFIRM_NEW_DISCARD").c_str(), ImVec2(90, 0))) {
                spdlog::info("NavigationService: confirm-new — user chose Discard");
                if (m_onNewProject) m_onNewProject();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button(tm->_t("DIALOG.CONFIRM_NEW_CANCEL").c_str(), ImVec2(90, 0))) {
                spdlog::info("NavigationService: confirm-new — user chose Cancel");
                ImGui::CloseCurrentPopup();
            }

            ADS::IDE::endModal();
        }
    }

    /**
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * First services any dialog already in flight: if its background thread
     * (see UI::AsyncFileDialog) has finished, forwards its result to the
     * registered callback. Then, if m_pendingOpenDialog/m_pendingSaveDialog is
     * set and that dialog isn't already running, starts it on a background
     * thread. Because this method is called after SDL_RenderPresent the
     * compositor already holds a clean frame when the dialog opens, and
     * because the NFD call itself never runs on the main thread, App::run()
     * keeps rendering new frames for as long as the dialog stays open.
     *
     * If a save dialog was triggered by the "New project" confirmation modal
     * (m_pendingSaveAndNew is set) and the user confirms a path, m_onNewProject
     * is called immediately after m_onSaveProject. If the user cancels the save
     * dialog neither callback is invoked and no project is created.
     */
    void NavigationService::processPendingDialogs()
    {
        std::optional<std::string> openResult;
        if (m_openDialog.poll(openResult)) {
            if (openResult) {
                spdlog::info("NavigationService: open path selected — {}", *openResult);
                if (m_onOpenProject) m_onOpenProject(*openResult);
            } else {
                spdlog::info("NavigationService: open dialog cancelled or failed");
            }
        }

        std::optional<std::string> saveResult;
        if (m_saveDialog.poll(saveResult)) {
            bool saveAndNew          = m_saveDialogWasSaveAndNew;
            m_saveDialogWasSaveAndNew = false;

            if (saveResult) {
                spdlog::info("NavigationService: save path selected — {}", *saveResult);
                if (m_onSaveProject) m_onSaveProject(*saveResult);
                if (saveAndNew && m_onNewProject) m_onNewProject();
            } else {
                spdlog::info("NavigationService: save dialog cancelled or failed");
            }
        }

        // Nothing to do on the vast majority of frames — bail before touching
        // the window handle or the (potentially expensive) providers.
        const bool openArmed = m_pendingOpenDialog && !m_openDialog.isRunning();
        const bool saveArmed = m_pendingSaveDialog && !m_saveDialog.isRunning();
        if (!openArmed && !saveArmed) {
            return;
        }

        nfdwindowhandle_t parentWindow = ADS::UI::getNfdParentWindowHandle(
            Core::App::getMainWindow() ? Core::App::getMainWindow()->getWindow() : nullptr);

        // Folder the pickers should open in (projects root). Resolved here on the
        // main thread — only now that a picker is actually being armed — and
        // captured by value into the background-thread lambdas.
        const std::string defaultDir = m_defaultDir ? m_defaultDir() : std::string{};

        if (openArmed) {
            m_pendingOpenDialog = false;

            m_openDialog.start([parentWindow, defaultDir]() -> std::optional<std::string> {
                nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };
                NFD::Guard guard;
                NFD::UniquePath outPath;
                nfdresult_t result = NFD::OpenDialog(
                    outPath, filters, 1,
                    defaultDir.empty() ? nullptr : defaultDir.c_str(), parentWindow);

                if (result == NFD_OKAY) return std::string(outPath.get());
                if (result == NFD_ERROR) spdlog::error("NavigationService: NFD error — {}", NFD::GetError());
                return std::nullopt;
            });
        }

        if (saveArmed) {
            m_pendingSaveDialog      = false;
            m_saveDialogWasSaveAndNew = m_pendingSaveAndNew;
            m_pendingSaveAndNew      = false;

            // Pre-fill the picker: the project's current path if it has one, else
            // the suggested <projects-root>/<slug>/<slug>.ads. Split into folder +
            // filename; fall back to the projects root + "project.ads".
            std::string saveFolder = defaultDir;
            std::string saveName   = "project.ads";
            if (m_getSavePrefill) {
                const std::string prefill = m_getSavePrefill();
                if (!prefill.empty()) {
                    const std::filesystem::path p(prefill);
                    if (p.has_parent_path()) saveFolder = p.parent_path().string();
                    if (p.has_filename())    saveName   = p.filename().string();
                }
            }

            m_saveDialog.start([parentWindow, saveFolder, saveName]() -> std::optional<std::string> {
                nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };
                NFD::Guard guard;
                NFD::UniquePath savePath;
                nfdresult_t result = NFD::SaveDialog(
                    savePath, filters, 1,
                    saveFolder.empty() ? nullptr : saveFolder.c_str(),
                    saveName.c_str(), parentWindow);

                if (result == NFD_OKAY) return std::string(savePath.get());
                if (result == NFD_ERROR) spdlog::error("NavigationService: NFD error — {}", NFD::GetError());
                return std::nullopt;
            });
        }
    }

    /**
     * @brief Whether an Open or Save dialog is currently running
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    bool NavigationService::isDialogInProgress() const
    {
        return m_openDialog.isRunning() || m_saveDialog.isRunning();
    }

} // ADS::IDE