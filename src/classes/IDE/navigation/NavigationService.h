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
         * @brief Pending-dialog flag
         *
         * Set to true by fileNewHandler() when a confirmation dialog must be
         * shown. Consumed by renderDialogs() via ImGui::OpenPopup().
         */
        bool m_confirmNewDialogOpen = false;

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
         * Opens a native OS file picker filtered to ADS project files (.ads).
         * If the user confirms a selection the path is forwarded to the onOpen
         * callback registered via setFileCallbacks(). A cancelled dialog is
         * silently ignored; an NFD error is logged via spdlog.
         *
         * @note This call blocks until the native dialog is dismissed
         * @see setFileCallbacks()
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
         * - **Save** — placeholder; will save then create (logs a warning for now)
         * - **Discard** — discards the current project and creates a new one
         * - **Cancel** — aborts the action, leaving the current project unchanged
         *
         * @note This method is a no-op when no dialog is pending
         */
        void renderDialogs();
    };
} // ADS::IDE
#endif //ADS_NAVIGATION_SERVICE_H