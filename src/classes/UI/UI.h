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
#include "fonts.h"
#include "imgui.h"
#include "uuid_v4.h"

namespace ADS::UI {
    class ImGuiManager {
    private:
        /**
         * Theme preference flag for the ImGui interface.
         * True for dark theme, false for light theme.
         * Controls the color scheme applied to all ImGui windows and widgets.
         */
        bool darkTheme;

        /**
         * Vector with fonts that would be loaded.
         */
        std::vector<std::string> fonts;

        /**
         * A set of windows that conformed the application
         */
        std::unordered_map<UUIDv4::UUID, Window*> windows;

        /**
         * Active Window
         */
        Window* activeWindow;

        /**
         * ImGui Configuration object and I/O handler
         */
        ImGuiIO* io;

        /**
         * Font manager for the application
         */
        Fonts* fontManager;

        /**
         * @brief Initialize SDL library and ImGui context
         *
         * Sets up SDL with video, timer, and game controller support.
         * Configures platform-specific settings (DPI awareness on Windows,
         * high-resolution displays on macOS) and creates the ImGui context.
         *
         * @throws UI::Exceptions::window_initialization_exception if SDL initialization fails
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

        /**
         * @brief Configure ImGui settings persistence
         *
         * Sets up the ImGui configuration file location and loads existing
         * settings if the file exists. This enables persistent UI state
         * across application sessions.
         */
        void setIniConfiguration() const;

        /**
         * @brief Configure ImGui I/O flags for input and rendering features
         *
         * Enables keyboard navigation, gamepad support, docking functionality,
         * and multi-viewport rendering. These flags must be set before the
         * first frame is rendered.
         */
        void setIOConfigFlags() const;

        /**
         * @brief Apply dark theme color scheme to ImGui
         *
         * Sets the internal theme flag and applies the dark color scheme
         * to all ImGui windows and widgets.
         */
        void setDarkTheme();

        /**
         * @brief Apply light theme color scheme to ImGui
         *
         * Sets the internal theme flag and applies the light color scheme
         * to all ImGui windows and widgets.
         */
        void setLightTheme();

        /**
         * @brief Set the active window by UUID
         *
         * Searches for a window with the given UUID and sets it as the
         * currently active window. If the UUID is not found, the active
         * window remains unchanged.
         *
         * @param uuid The unique identifier of the window to set as active
         *
         * @see getWindowFromId(), getActiveWindow()
         */
        void setActiveWindow(UUIDv4::UUID uuid);

        /**
         * @brief Retrieve the currently active window
         *
         * Returns a pointer to the window that is currently set as active.
         * If no active window has been explicitly set, returns the first
         * window in the windows collection.
         *
         * @return Window* Pointer to the active window
         *
         * @see setActiveWindow()
         */
        Window* getActiveWindow();

        /**
         * @brief Get the font manager instance
         *
         * Returns a pointer to the Fonts manager that handles all font
         * loading and retrieval for the application.
         *
         * @return Fonts* Pointer to the font manager
         *
         * @see Fonts
         */
        Fonts* getFontManager() const;

        /**
         * @brief Get the ImGui I/O context
         *
         * Returns a pointer to the ImGui I/O context for direct access
         * when needed (e.g., for rendering operations).
         *
         * @return ImGuiIO* Pointer to the ImGui I/O context
         */
        ImGuiIO* getIO() const;

    };
} // ADS::UI

#endif //ADVENTURE_DESIGNER_STUDIO_IM_GUI_H