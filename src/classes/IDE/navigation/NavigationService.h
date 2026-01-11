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

namespace ADS::IDE {
    /**
     * @brief Service for handling navigation and file operation actions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides centralized handling of navigation-related actions triggered from
     * menu items and toolbar buttons. Currently implements file operation handlers
     * for New and Open actions. This service class decouples UI components from
     * action implementation logic.
     */
    class NavigationService
    {

    public:
        /**
         * @brief Construct a new NavigationService object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default constructor. Initializes the navigation service with no
         * additional setup required.
         */
        NavigationService() = default;

        /**
         * @brief Handle the File Open action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Handles the File Open action triggered from menu items or toolbar buttons.
         * Currently logs the action via spdlog for debugging purposes. Future
         * implementation will open a file dialog and load the selected file.
         *
         * @note Currently contains placeholder implementation (logging only)
         */
        void fileOpenHandler();

        /**
         * @brief Handle the File New action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Handles the File New action triggered from menu items or toolbar buttons.
         * Currently logs the action via spdlog for debugging purposes. Future
         * implementation will create a new file or project.
         *
         * @note Currently contains placeholder implementation (logging only)
         */
        void fileNewHandler();

    };
} // ADS::IDE
#endif //ADS_NAVIGATION_SERVICE_H