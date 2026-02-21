/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
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

        NFD::Guard guard;
        NFD::UniquePath outPath;

        nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };
        nfdresult_t result = NFD::OpenDialog(outPath, filters, 1);

        if (result == NFD_OKAY) {
            spdlog::info("NavigationService: open path selected — {}", outPath.get());
            if (m_onOpenProject) m_onOpenProject(outPath.get());
        } else if (result == NFD_ERROR) {
            spdlog::error("NavigationService: NFD error — {}", NFD::GetError());
        }
        // NFD_CANCEL: user dismissed the dialog — nothing to do
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
                NFD::Guard guard;
                NFD::UniquePath savePath;

                nfdfilteritem_t filters[] = { { "ADS Project", "ads" } };
                nfdresult_t result = NFD::SaveDialog(
                    savePath, filters, 1, nullptr, "project.ads");

                if (result == NFD_OKAY) {
                    spdlog::info("NavigationService: save path selected — {}",
                                 savePath.get());
                    if (m_onSaveProject) m_onSaveProject(savePath.get());
                    if (m_onNewProject)  m_onNewProject();
                    ImGui::CloseCurrentPopup();
                } else if (result == NFD_ERROR) {
                    spdlog::error("NavigationService: NFD error — {}",
                                  NFD::GetError());
                }
                // NFD_CANCEL: user dismissed the save dialog — stay in the modal
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

} // ADS::IDE