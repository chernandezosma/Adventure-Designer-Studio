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


#ifndef ADS_IM_GUI_H
#define ADS_IM_GUI_H

#include <SDL_render.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>

#include "Window.h"
#include "fonts.h"
#include "imgui.h"

// Forward declaration to avoid circular dependency
namespace ADS::IDE {
    class Theme;
}

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
        std::unordered_map<boost::uuids::uuid, Window*, boost::hash<boost::uuids::uuid>> windows;

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
         * Current active theme for the IDE
         */
        IDE::Theme* currentTheme;

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
         * @brief Construct a new ImGuiManager
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Initializes the ImGui manager with empty font and window collections.
         * Sets up the internal data structures needed to manage multiple windows
         * and font resources throughout the application lifecycle.
         */
        explicit ImGuiManager();

        /**
         * @brief Create a new window and add it to the managed windows collection
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Creates a new Window instance using the provided configuration and
         * registers it in the internal windows collection for lifecycle management.
         * The window is allocated on the heap and a unique UUID is generated for it.
         *
         * @param windowInfo Pointer to SDL window configuration structure
         * @param flags Flags to apply to the new window and its renderer handler
         * @return std::pair<boost::uuids::uuid, Window*> Generated UUID and pointer to the new window
         */
        std::pair<boost::uuids::uuid, Window*> newWindow(const SDL_WINDOW_INFO* windowInfo, SDL_FLAGS* flags);

        /**
         * @brief Retrieve a window from the collection by its UUID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param uuid The unique identifier of the window to retrieve
         * @return Window* Pointer to the window if found, nullptr otherwise
         */
        Window* getWindowFromId(const boost::uuids::uuid& uuid);

        /**
         * @brief Configure ImGui settings persistence
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Sets up the ImGui configuration file location and loads existing
         * settings if the file exists. This enables persistent UI state
         * across application sessions.
         */
        void setIniConfiguration() const;

        /**
         * @brief Configure ImGui I/O flags for input and rendering features
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Enables keyboard navigation, gamepad support, docking functionality,
         * and multi-viewport rendering. These flags must be set before the
         * first frame is rendered.
         */
        void setIOConfigFlags() const;

        /**
         * @brief Apply dark theme color scheme to ImGui
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Sets the internal theme flag and applies the dark color scheme
         * to all ImGui windows and widgets.
         */
        void setDarkTheme();

        /**
         * @brief Apply light theme color scheme to ImGui
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Sets the internal theme flag and applies the light color scheme
         * to all ImGui windows and widgets.
         */
        void setLightTheme();

        /**
         * @brief Set the active window by UUID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Searches for a window with the given UUID and sets it as the
         * currently active window. If the UUID is not found, the active
         * window remains unchanged.
         *
         * @param uuid The unique identifier of the window to set as active
         */
        void setActiveWindow(boost::uuids::uuid uuid);

        /**
         * @brief Get the currently active window
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns a pointer to the window that is currently set as active.
         * If no active window has been explicitly set, returns the first
         * window in the windows collection.
         *
         * @return Window* Pointer to the active window
         */
        Window* getActiveWindow();

        /**
         * @brief Get the font manager instance
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return Fonts* Pointer to the font manager
         */
        Fonts* getFontManager() const;

        /**
         * @brief Get the ImGui I/O context
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return ImGuiIO* Pointer to the ImGui I/O context
         */
        ImGuiIO* getIO() const;

        /**
         * @brief Get the current active theme
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns a pointer to the currently active theme instance.
         * This can be used to query theme properties or apply the theme.
         *
         * @return IDE::Theme* Pointer to current theme
         */
        IDE::Theme* getCurrentTheme() const;

    };
} // ADS::UI

#endif //ADVENTURE_DESIGNER_STUDIO_IM_GUI_H