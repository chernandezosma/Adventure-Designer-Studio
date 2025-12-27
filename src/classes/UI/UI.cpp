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


#include "UI.h"
#include <SDL.h>
#include <filesystem>

#include "System.h"
#include "Window.h"
#include "adsString.h"
#include "app.h"
#include "imgui.h"
#include "imgui/window_intialization_exception.h"
#include "spdlog/spdlog.h"
#include "../IDE/themes/Theme.h"
#include "../IDE/themes/DarkTheme.h"
#include "../IDE/themes/LightTheme.h"

namespace ADS::UI {
    /**
     * @brief Initialize SDL library with platform-specific configurations
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Sets up SDL for video, timer, and game controller support with
     * platform-specific optimizations. On Windows, enables DPI awareness.
     * On macOS, configures high-resolution display support. Also enables
     * native IME (Input Method Editor) support if available.
     *
     * @throws UI::Exceptions::window_initialization_exception if SDL initialization fails
     */
    void ImGuiManager::init()
    {
        // Windows - DPI Awareness
#ifdef _WIN32
        ::SetProcessDPIAware();
#endif
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
            0) {
            auto message = std::format("{0}:{1} - Error: {2}", __FILE__, __LINE__,
                                       SDL_GetError());
            spdlog::error(message);
            throw Imgui::Exceptions::window_initialization_exception(message);
        }

        // macOS - High resolution configuration
#ifdef __APPLE__
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
#endif

        // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

        // Setup Dear ImGui context
        ::ImGui::DebugCheckVersionAndDataLayout(
                IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2),
                sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
        ::ImGui::CreateContext();
        this->io = &::ImGui::GetIO();

        // Initialize font manager after io is ready
        this->fontManager = new Fonts(this->io);
        this->fontManager->loadDefaultFonts();

        this->setIniConfiguration();
        this->setIOConfigFlags();

        // Initialize theme based on preference
        if (this->darkTheme) {
            this->currentTheme = new IDE::DarkTheme();
        } else {
            this->currentTheme = new IDE::LightTheme();
        }

        // Apply the theme
        this->currentTheme->apply();

        // [Experimental] Automatically overwrite style.FontScaleDpi
        // in Begin() when Monitor DPI changes. This will scale
        // fonts but _NOT_ scale sizes/padding for now.
        this->io->ConfigDpiScaleFonts = true;

        // [Experimental] Scale Dear ImGui and Platform Windows
        // when Monitor DPI changes.
        this->io->ConfigDpiScaleViewports = true;
    }

    /**
     * @brief Constructor for ImGuiManager class
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Initializes the ImGui manager with empty font and window collections.
     * Sets up the internal data structures needed to manage multiple windows
     * and font resources throughout the application lifecycle.
     */
    ImGuiManager::ImGuiManager()
    {
        this->darkTheme = true;  // Use dark theme by default
        this->fonts = std::vector<std::string>();
        this->windows = std::unordered_map<UUIDv4::UUID, Window *>();
        this->io = nullptr;
        this->fontManager = nullptr;
        this->currentTheme = nullptr;
        this->init();
    }

    /**
     * @brief Create a new window and add it to the managed windows collection
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Creates a new Window instance using the provided configuration and
     * registers it in the internal windows collection for lifecycle management.
     * The window is allocated on the heap and ownership is retained by this
     * ImGuiManager instance.
     *
     * @param windowInfo Pointer to SDL window configuration structure containing
     *                   title, position (x, y), and dimensions (width, height)
     * @param flags Pointer to SDL_FLAGS structure containing window and renderer flags
     *
     * @return Window* Pointer to the newly created window instance
     *
     * @see Window
     */
    std::pair<UUIDv4::UUID, Window *> ImGuiManager::newWindow(
            const SDL_WINDOW_INFO *windowInfo, SDL_FLAGS *flags)
    {
        Window *window = new Window(
                windowInfo->title,
                windowInfo->x,
                windowInfo->y,
                windowInfo->width,
                windowInfo->height,
                flags,
                this->io);
        UUIDv4::UUID uuid = getRandomUuid();
        this->windows.insert({uuid, window});

        return std::make_pair(uuid, window);
    }

    /**
     * @brief Retrieve a window from the collection by its UUID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Searches for a window in the internal windows collection using its
     * unique identifier. Returns the window pointer if found, or nullptr
     * if the UUID does not exist in the collection.
     *
     * @param uuid The unique identifier of the window to retrieve
     *
     * @return Window* Pointer to the window if found, nullptr otherwise
     *
     * @note This function does not throw exceptions on failure
     * @see newWindow()
     */
    Window *ImGuiManager::getWindowFromId(const UUIDv4::UUID &uuid)
    {
        auto it = this->windows.find(uuid);

        if (it != this->windows.end()) {
            return it->second;
        }

        return nullptr;
    }

    /**
     * @brief Configure ImGui settings persistence
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Sets up the ImGui configuration file location and loads existing
     * settings if the file exists. This enables persistent UI state
     * (window positions, sizes, docking layouts) across application sessions.
     * The configuration file path is read from system constants.
     *
     * @note Only executes if the ImGui I/O context is properly initialized
     * @see setIOConfigFlags()
     */
    void ImGuiManager::setIniConfiguration() const
    {
        if (this->io != nullptr) {
            this->io->IniFilename = Constants::System::CONFIG_FILE;
            if (std::filesystem::exists(Constants::System::CONFIG_FILE)) {
                ImGui::LoadIniSettingsFromDisk(Constants::System::CONFIG_FILE);
            }
        }
    }

    /**
     * @brief Configure ImGui I/O flags for input and rendering features
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Enables essential ImGui features by setting configuration flags:
     * - Keyboard navigation support for accessibility and usability
     * - Gamepad/controller input for console-style navigation
     * - Docking system for flexible window layout management
     * - Multi-viewport rendering for windows outside main viewport
     *
     * These flags must be set after context creation but before the first
     * frame is rendered. Changes to these flags during runtime may cause
     * undefined behavior.
     *
     * @note This method assumes the ImGui I/O context is already initialized
     * @see init(), setIniConfiguration()
     */
    void ImGuiManager::setIOConfigFlags() const
    {
        this->io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
        this->io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
        this->io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;        // Enable Docking
        this->io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows
    }

    /**
     * @brief Apply dark theme color scheme to ImGui
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Sets the internal theme flag to true and applies ImGui's built-in
     * dark color scheme to all windows and widgets. This change takes
     * effect immediately for all rendered UI elements.
     */
    void ImGuiManager::setDarkTheme()
    {
        this->darkTheme = true;
        delete this->currentTheme;
        this->currentTheme = new IDE::DarkTheme();
        this->currentTheme->apply();
    }

    /**
     * @brief Apply light theme color scheme to ImGui
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Sets the internal theme flag to false and applies ImGui's built-in
     * light color scheme to all windows and widgets. This change takes
     * effect immediately for all rendered UI elements.
     */
    void ImGuiManager::setLightTheme()
    {
        this->darkTheme = false;
        delete this->currentTheme;
        this->currentTheme = new IDE::LightTheme();
        this->currentTheme->apply();
    }

    /**
     * @brief Set the active window by UUID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Searches for a window with the given UUID in the managed windows
     * collection and sets it as the currently active window. If the UUID
     * is not found in the collection, the active window remains unchanged.
     * This method is useful for managing focus and operations on multiple
     * windows within the application.
     *
     * @param uuid The unique identifier of the window to set as active
     *
     * @see getWindowFromId(), getActiveWindow()
     */
    void ImGuiManager::setActiveWindow(UUIDv4::UUID uuid)
    {
        Window* window = this->getWindowFromId(uuid);
        if (window != nullptr) {
            this->activeWindow = window;
        }
    }

    /**
     * @brief Retrieve the currently active window
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns a pointer to the window that is currently set as active.
     * If no active window has been explicitly set via setActiveWindow(),
     * this method defaults to returning the first window in the windows
     * collection.
     *
     * @return Window* Pointer to the active window
     *
     * @note This method modifies the internal active window state before returning
     * @see setActiveWindow()
     */
    Window* ImGuiManager::getActiveWindow()
    {
        this->activeWindow = this->windows.begin()->second;

        return this->activeWindow;
    }

    /**
     * @brief Get the font manager instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns a pointer to the Fonts manager that handles all font
     * loading and retrieval for the application. Use this to load
     * custom fonts or retrieve loaded fonts by name.
     *
     * @return Fonts* Pointer to the font manager
     *
     * @see Fonts, getIO()
     */
    Fonts* ImGuiManager::getFontManager() const
    {
        return this->fontManager;
    }

    /**
     * @brief Get the ImGui I/O context
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns a pointer to the ImGui I/O context for direct access
     * when needed for rendering operations, checking input state,
     * or accessing display properties.
     *
     * @return ImGuiIO* Pointer to the ImGui I/O context
     *
     * @see getFontManager()
     */
    ImGuiIO* ImGuiManager::getIO() const
    {
        return this->io;
    }

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
     *
     * @see setDarkTheme(), setLightTheme()
     */
    IDE::Theme* ImGuiManager::getCurrentTheme() const
    {
        return this->currentTheme;
    }

} // ADS::UI
