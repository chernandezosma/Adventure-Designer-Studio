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

#include "Window.h"
#include "adsString.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui/window_intialization_exception.h"
#include "spdlog/spdlog.h"

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
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
            auto message = std::format("{0}:{1} - Error: {2}", __FILE__, __LINE__, SDL_GetError());
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
        ::ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
        ::ImGui::CreateContext();
        this->io = &::ImGui::GetIO();
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
        this->fonts = std::vector<std::string>();
        this->windows = std::unordered_map<UUIDv4::UUID, Window *>();
        this->io = nullptr;
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
    std::pair<UUIDv4::UUID, Window*> ImGuiManager::newWindow(const SDL_WINDOW_INFO* windowInfo, SDL_FLAGS* flags)
    {
        Window* window = new Window(
            windowInfo->title,
            windowInfo->x,
            windowInfo->y,
            windowInfo->width,
            windowInfo->height,
            flags
        );

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
    Window* ImGuiManager::getWindowFromId(const UUIDv4::UUID& uuid)
    {
        auto it = this->windows.find(uuid);

        if (it != this->windows.end()) {
            return it->second;
        }

        return nullptr;
    }


} // ADS::UI