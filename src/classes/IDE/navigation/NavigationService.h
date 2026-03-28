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


#ifndef ADS_NAVIGATION_SERVICE_H
#define ADS_NAVIGATION_SERVICE_H

#include <functional>
#include <string>

namespace ADS::IDE {
    /**
     * @brief Service for handling navigation and file operation actions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides centralized handling of navigation-related actions triggered from
     * menu items and toolbar buttons. Implements file operation handlers for New
     * and Open actions. This service class decouples UI components from action
     * implementation logic.
     *
     * When a destructive action (e.g. File > New) is triggered while a project is
     * already open, the service raises an ImGui modal confirmation dialog asking
     * the user whether to save, discard, or cancel before proceeding.
     * Call renderDialogs() once per frame from within an active ImGui window to
     * service any pending modal.
     */
    class NavigationService
    {
    private:
        /**
         * @brief Callback that returns true when a project is currently open
         *
         * Set via setProjectCallbacks(). If nullptr the service assumes no
         * project is active and skips the confirmation dialog.
         */
        std::function<bool()> m_hasActiveProject;

        /**
         * @brief Callback invoked to create a fresh empty project
         *
         * Set via setProjectCallbacks(). Called after the user confirms they
         * are willing to discard the current project.
         */
        std::function<void()> m_onNewProject;

        /**
         * @brief Callback invoked when the user selects a file to open
         *
         * Receives the absolute path chosen by the user in the open dialog.
         * Set via setFileCallbacks(). If nullptr the path is only logged.
         */
        std::function<void(const std::string&)> m_onOpenProject;

        /**
         * @brief Callback invoked when the user selects a path to save to
         *
         * Receives the absolute path chosen by the user in the save dialog.
         * Set via setFileCallbacks(). If nullptr the path is only logged.
         */
        std::function<void(const std::string&)> m_onSaveProject;

        /**
         * @brief Pending-dialog flag for the ImGui "New project" confirmation modal
         *
         * Set to true by fileNewHandler() when a confirmation dialog must be
         * shown. Consumed by renderDialogs() via ImGui::OpenPopup().
         */
        bool m_confirmNewDialogOpen = false;

        /**
         * @brief Deferred open-dialog flag
         *
         * Set by fileOpenHandler() instead of calling NFD inline.
         * Consumed by processPendingDialogs() after the next SDL_RenderPresent,
         * ensuring the compositor has a clean frame before the blocking call.
         */
        bool m_pendingOpenDialog = false;

        /**
         * @brief Deferred save-dialog flag
         *
         * Set by the Save button inside renderDialogs() instead of calling NFD
         * inline. Consumed by processPendingDialogs() after the next
         * SDL_RenderPresent.
         */
        bool m_pendingSaveDialog = false;

        /**
         * @brief Whether a pending save should also trigger a new project
         *
         * Set alongside m_pendingSaveDialog when the save originates from the
         * "New project" confirmation modal. If the user confirms a save path,
         * m_onNewProject is called after m_onSaveProject.
         */
        bool m_pendingSaveAndNew = false;

        /** ImGui popup identifier used for the "new project" confirmation modal */
        static constexpr const char* NEW_PROJECT_POPUP_ID = "New project##ads_confirm_new";

    public:
        /**
         * @brief Construct a new NavigationService object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default constructor. Initializes the navigation service with no
         * additional setup required. Call setProjectCallbacks() before using
         * fileNewHandler().
         */
        NavigationService() = default;

        /**
         * @brief Register project-awareness callbacks
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides the service with two lightweight callbacks so that it can
         * check whether a project is currently loaded and trigger the creation
         * of a new one, without taking a direct dependency on IDERenderer or
         * Core::Project.
         *
         * @param hasProject  Predicate that returns true when a project is open
         * @param onNewProject Callable invoked when a new project should be created
         */
        void setProjectCallbacks(
            std::function<bool()> hasProject,
            std::function<void()> onNewProject
        );

        /**
         * @brief Register file I/O callbacks used by the native file dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides the service with two callbacks so that the result of a native
         * file dialog can be forwarded to the application without the service
         * holding a direct reference to IDERenderer or any persistence layer.
         *
         * @param onOpen  Callable receiving the absolute path the user selected
         *                in the Open dialog; called only on NFD_OKAY
         * @param onSave  Callable receiving the absolute path the user selected
         *                in the Save dialog; called only on NFD_OKAY
         */
        void setFileCallbacks(
            std::function<void(const std::string&)> onOpen,
            std::function<void(const std::string&)> onSave
        );

        /**
         * @brief Handle the File Open action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Schedules a native OS file picker by setting an internal pending flag.
         * The actual NFD call is deferred to processPendingDialogs(), which must
         * be called after SDL_RenderPresent to avoid the gray-window artifact that
         * occurs when the render loop is blocked mid-frame.
         *
         * @note Does not block; the dialog opens on the next processPendingDialogs() call
         * @see processPendingDialogs(), setFileCallbacks()
         */
        void fileOpenHandler();

        /**
         * @brief Handle the File New action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * If no project is currently open, creates a new project immediately
         * by invoking the onNewProject callback. If a project is already open,
         * schedules a confirmation modal that asks the user whether to save,
         * discard, or cancel before discarding the current work.
         *
         * The modal is rendered on the next frame(s) via renderDialogs().
         *
         * @see setProjectCallbacks()
         * @see renderDialogs()
         */
        void fileNewHandler();

        /**
         * @brief Render any pending modal dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Must be called once per frame from within an active ImGui window
         * (outside any BeginMenu/EndMenu scope). Services the confirmation
         * dialog that may have been scheduled by fileNewHandler().
         *
         * The dialog presents three choices:
         * - **Save**    — schedules a native save dialog via processPendingDialogs()
         * - **Discard** — discards the current project and creates a new one
         * - **Cancel**  — aborts the action, leaving the current project unchanged
         *
         * @note This method is a no-op when no dialog is pending
         * @see processPendingDialogs()
         */
        void renderDialogs();

        /**
         * @brief Execute any deferred native file dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Must be called **after** SDL_RenderPresent (i.e. after render() returns)
         * and **before** the next ImGui::NewFrame(). At that point the compositor
         * already holds a clean frame so blocking the main thread with a native
         * dialog does not produce a gray window.
         *
         * Consumes m_pendingOpenDialog and m_pendingSaveDialog. For each flag that
         * is set it opens the corresponding NFD dialog, forwards the result to the
         * registered callback, and resets the flag.
         *
         * @note This method is a no-op when no dialog is pending
         * @see fileOpenHandler(), renderDialogs()
         */
        void processPendingDialogs();
    };
} // ADS::IDE
#endif //ADS_NAVIGATION_SERVICE_H