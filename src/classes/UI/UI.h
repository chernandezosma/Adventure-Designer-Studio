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


#ifndef ADS_IM_GUI_H
#define ADS_IM_GUI_H

#include <SDL_render.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "Window.h"
#include "imgui.h"
#include "uuid_v4.h"

namespace ADS::UI {
    class ImGuiManager {
    private:

        /**
         * Vector with fonts that would be loaded.
         */
        std::vector<std::string> fonts;

        /**
         * A set of windows that conformed the application
         */
        std::unordered_map<UUIDv4::UUID, Window*> windows;

        /**
         * ImGui Configuration object...
         */
        ImGuiIO* io;

        /**
         * SDL library initialization
         */
        void init();

    public:
        /**
         * @brief Constructor for ImGuiManager class
         *
         * Initializes the ImGui manager with empty font and window collections.
         * Sets up the internal data structures needed to manage multiple windows
         * and font resources throughout the application lifecycle.
         */
        explicit ImGuiManager();

        /**
         * @brief Create a new window and add it to the managed windows collection
         *
         * Creates a new Window instance using the provided configuration and
         * registers it in the internal windows collection for lifecycle management.
         * The window is allocated on the heap and a unique UUID is generated for it.
         *
         * @param windowInfo    Pointer to SDL window configuration structure containing
         *                      title, position (x, y), and dimensions (width, height)
         * @param flags         Flags to applied to the new window and its renderer handler
         *
         * @return std::pair<UUIDv4::UUID, Window*> Pair containing the generated UUID
         *                                          and pointer to the newly created window
         *
         * @see Window, getWindowFromId()
         */
        std::pair<UUIDv4::UUID, Window*> newWindow(const SDL_WINDOW_INFO* windowInfo, SDL_FLAGS* flags);

        /**
         * @brief Retrieve a window from the collection by its UUID
         *
         * @param uuid The unique identifier of the window to retrieve
         * @return Window* Pointer to the window if found, nullptr otherwise
         */
        Window* getWindowFromId(const UUIDv4::UUID& uuid);

    };
} // ADS::UI

#endif //ADVENTURE_DESIGNER_STUDIO_IM_GUI_H