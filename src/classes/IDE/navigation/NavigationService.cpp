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
#include "spdlog/spdlog.h"

namespace ADS::IDE {

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
    }

    /**
     * @brief Handle the File New action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the File New action triggered from menu items or toolbar buttons.
     * Currently logs the action to the console using spdlog for debugging and
     * verification purposes.
     *
     * Future implementation will:
     * - Create a new file or project
     * - Initialize default content/structure
     * - Update the IDE state with the new file
     * - Optionally prompt for save location
     *
     * @note Currently contains placeholder implementation (logging only)
     * @see MenuBarRenderer::renderFileMenu()
     * @see ToolBarRenderer::renderFileButtons()
     */
    void NavigationService::fileNewHandler()
    {
        spdlog::info("Call NavigationService::fileNewHandler");
    }
} // ADS::IDE