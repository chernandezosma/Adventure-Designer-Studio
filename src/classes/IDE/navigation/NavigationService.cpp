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

        // Centre the modal on screen
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(NEW_PROJECT_POPUP_ID, nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("A project is already open.");
            ImGui::Text("Do you want to save it before creating a new one?");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Save", ImVec2(90, 0))) {
                // Defer the NFD save dialog to processPendingDialogs().
                // The confirm modal closes now so the compositor gets a clean
                // frame before the blocking call.
                m_pendingSaveDialog  = true;
                m_pendingSaveAndNew  = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button("Discard", ImVec2(90, 0))) {
                if (m_onNewProject) m_onNewProject();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(90, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    /**
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Checks m_pendingOpenDialog and m_pendingSaveDialog. For each flag that
     * is set it opens the corresponding NFD dialog synchronously, forwards the
     * result to the registered callback, and resets the flag. Because this
     * method is called after SDL_RenderPresent the compositor already holds a
     * clean frame, so blocking the thread here does not produce a gray window.
     *
     * If a save dialog was triggered by the "New project" confirmation modal
     * (m_pendingSaveAndNew is set) and the user confirms a path, m_onNewProject
     * is called immediately after m_onSaveProject. If the user cancels the save
     * dialog neither callback is invoked and no project is created.
     */
    void NavigationService::processPendingDialogs()
    {
        nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };

        if (m_pendingOpenDialog) {
            m_pendingOpenDialog = false;

            NFD::Guard guard;
            NFD::UniquePath outPath;
            nfdresult_t result = NFD::OpenDialog(outPath, filters, 1);

            if (result == NFD_OKAY) {
                spdlog::info("NavigationService: open path selected — {}",
                             outPath.get());
                if (m_onOpenProject) m_onOpenProject(outPath.get());
            } else if (result == NFD_ERROR) {
                spdlog::error("NavigationService: NFD error — {}", NFD::GetError());
            }
            // NFD_CANCEL: user dismissed the dialog — nothing to do
        }

        if (m_pendingSaveDialog) {
            m_pendingSaveDialog = false;
            bool saveAndNew     = m_pendingSaveAndNew;
            m_pendingSaveAndNew = false;

            NFD::Guard guard;
            NFD::UniquePath savePath;
            nfdresult_t result = NFD::SaveDialog(
                savePath, filters, 1, nullptr, "project.ads");

            if (result == NFD_OKAY) {
                spdlog::info("NavigationService: save path selected — {}",
                             savePath.get());
                if (m_onSaveProject) m_onSaveProject(savePath.get());
                if (saveAndNew && m_onNewProject) m_onNewProject();
            } else if (result == NFD_ERROR) {
                spdlog::error("NavigationService: NFD error — {}", NFD::GetError());
            }
            // NFD_CANCEL: user cancelled — neither save nor new-project proceeds
        }
    }

} // ADS::IDE