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
#include <optional>
#include <string>

#include "UI/AsyncFileDialog.h"

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
         * @brief Callback returning the project's current file path, if any
         *
         * Set via setProjectPathProvider(). Lets fileSaveHandler() save straight
         * to a known path without a dialog; returns std::nullopt for a project
         * that has never been saved.
         */
        std::function<std::optional<std::string>()> m_getProjectPath;

        /**
         * @brief Callback returning the folder the native pickers should open in
         *
         * Set via setDefaultDirProvider(). Resolved to the projects root
         * (<home>/ADS-Projects or $PROJECTS_DIR) so Open / Save As start there.
         * An empty string means "let the OS decide".
         */
        std::function<std::string()> m_defaultDir;

        /**
         * @brief Callback returning the full path the save picker should pre-fill
         *
         * Set via setSavePrefillProvider(). The native Save / Save As dialog
         * splits this into a starting folder + filename. For a saved project it
         * is the current file path; for an unsaved one it is a suggested
         * `<projects-root>/<slug>/<slug>.ads`. Empty string means no hint.
         */
        std::function<std::string()> m_getSavePrefill;

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

        /**
         * @brief Background runner for the Open dialog's blocking NFD call
         *
         * NFD::OpenDialog() blocks for the dialog's entire lifetime. Running
         * it on the main thread would stall processEvents()/render(), so the
         * parent window never repaints while the dialog is dragged. Running
         * it here instead lets processPendingDialogs() keep polling every
         * frame while App::run() keeps presenting new frames in the meantime.
         * @see UI::AsyncFileDialog
         */
        UI::AsyncFileDialog m_openDialog;

        /** Background runner for the Save dialog's blocking NFD call; see m_openDialog. */
        UI::AsyncFileDialog m_saveDialog;

        /**
         * @brief Captured value of m_pendingSaveAndNew at the moment the async save dialog started
         *
         * m_pendingSaveAndNew is cleared as soon as the dialog is launched (it
         * may be set again by a later, unrelated Save action before this one's
         * background thread finishes), so the value needed once the result
         * arrives is kept here instead.
         */
        bool m_saveDialogWasSaveAndNew = false;

        /** ImGui popup identifier used for the "new project" confirmation modal */
        // Stable id ("###..."): ADS::IDE::beginModal() prepends the visible caption.
        static constexpr const char* NEW_PROJECT_POPUP_ID = "###ads_confirm_new";

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
         * @brief Whether a project is currently open
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Evaluates the predicate registered via setProjectCallbacks(). Used by
         * the menu / toolbar to disable project-scoped actions (Save, Save As)
         * while the IDE sits on its empty start screen.
         *
         * @return bool true when a project is loaded
         */
        [[nodiscard]] bool hasActiveProject() const {
            return m_hasActiveProject && m_hasActiveProject();
        }

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
         * @brief Register the provider for the project's current file path
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param getPath Callable returning the saved path, or std::nullopt when
         *                the project has never been saved
         */
        void setProjectPathProvider(std::function<std::optional<std::string>()> getPath);

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
        void setDefaultDirProvider(std::function<std::string()> getDir);

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
        void setSavePrefillProvider(std::function<std::string()> getPrefill);

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
        void fileSaveHandler();

        /**
         * @brief Handle the File Save As action — always shows the save picker
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        void fileSaveAsHandler();

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
         * already holds a clean frame so launching the dialog does not produce a
         * gray window.
         *
         * Non-blocking: when m_pendingOpenDialog/m_pendingSaveDialog is set, the
         * actual NFD call is started on a background thread (see
         * UI::AsyncFileDialog) so the main loop keeps rendering while the dialog
         * is open — this is what keeps the parent window repainting while the
         * user drags the dialog. Each call also polls any dialog already in
         * flight and, once it has finished, forwards its result to the
         * registered callback.
         *
         * @note This method is a no-op when no dialog is pending or in flight
         * @see fileOpenHandler(), renderDialogs(), UI::AsyncFileDialog
         */
        void processPendingDialogs();

        /**
         * @brief Whether an Open or Save dialog is currently running
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * NFD's blocking call has no cancel/close API, and the dialog window
         * itself belongs to a separate portal process (not this app), so it
         * cannot be force-closed from here. Callers should use this to avoid
         * quitting the app while a dialog is in flight, which would leave it
         * orphaned on screen with no owning process.
         *
         * @return true if a background dialog thread is currently running
         */
        bool isDialogInProgress() const;
    };
} // ADS::IDE
#endif //ADS_NAVIGATION_SERVICE_H