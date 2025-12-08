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

#include "Window.h"
#include <string>

#include "SDL_video.h"

#include "imgui_impl_sdl2.h"
#include "spdlog/spdlog.h"

namespace ADS::UI {
    /**
     * @brief Construct a new Window instance with SDL backend
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Creates an SDL window with the specified dimensions and title. The constructor
     * performs the following steps:
     * 1. Combines provided flags with DEFAULT_FLAGS
     * 2. Retrieves the content scale factor for high-DPI displays
     * 3. Creates the SDL window with scaled dimensions
     * 4. Validates window creation and throws exception on failure
     *
     * @param title Window title displayed in the title bar
     * @param x X position of the window
     * @param y Y position of the window
     * @param width Window width in logical units (will be scaled by DPI factor)
     * @param height Window height in logical units (will be scaled by DPI factor)
     * @param flags Additional SDL window flags to combine with defaults
     *
     * @throws std::runtime_error if SDL_CreateWindow fails
     *
     * @note Window dimensions are automatically scaled based on display DPI
     */
    Window::Window(std::string title, float x, float y, float width, float height, SDL_FLAGS* flags) {
        this->flags = new SDL_FLAGS();

        this->flags->windowFlags = static_cast<SDL_WindowFlags>(Window::DEFAULT_FLAGS | flags->windowFlags);
        this->mainScale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
        this->window = SDL_CreateWindow(title.data(), x, y, width * this->mainScale, height * this->mainScale, this->flags->windowFlags);

        if (this->window == nullptr) {
            auto errorMessage = std::format("{}:{} Error: SDL_CreateWindow(): {}\n", __FILE__, __LINE__, SDL_GetError());
            spdlog::error(errorMessage);
            throw std::runtime_error(std::format("Failed to create window: {}", errorMessage));
        }

        this->flags->rendererFlags = this->getDefaultRenderFlags() | flags->rendererFlags;
        this->renderer = this->createRenderer();
    }

    /**
     * @brief Construct a new Window from SDL_WINDOW_INFO structure
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Convenience constructor that delegates to the main constructor by unpacking
     * the SDL_WINDOW_INFO structure fields. This provides a cleaner interface when
     * window configuration is already encapsulated in the SDL_WINDOW_INFO struct.
     *
     * @param info Pointer to SDL_WINDOW_INFO containing title, position, size, and flags
     *
     * @throws std::runtime_error if SDL window creation fails (delegated from main constructor)
     *
     * @note This constructor uses constructor delegation (C++11 feature)
     * @see SDL_WINDOW_INFO, Window(std::string, float, float, float, float, SDL_WindowFlags)
     */
    Window::Window(SDL_WINDOW_INFO *info, SDL_FLAGS* flags)
        : Window(info->title, info->x, info->y, info->width, info->height, flags) {
    }

    /**
     * @brief Create a 2D rendering context for a window.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Create a 2D rendering context for a window.
     *
     * @param index         The index of the rendering driver to initialize, or -1 to
     *                      initialize the first one supporting the requested flags.
     * @param rendererFlags Flags which qualified the renderer for current window
     *
     * @throws std::runtime_error if SDL window creation fails (delegated from main constructor)
     *
     * @note This constructor uses constructor delegation (C++11 feature)
     * @see SDL_WINDOW_INFO, Window(std::string, float, float, float, float, SDL_WindowFlags)
     */
    SDL_Renderer *Window::createRenderer(int index) const {
        SDL_Renderer *renderer = SDL_CreateRenderer(this->getWindow(), index, this->flags->rendererFlags);
        if (renderer == nullptr) {
            spdlog::error(std::format("{}:{} Error creating renderer: {}\n", __FILE__, __LINE__, SDL_GetError()));
            throw std::runtime_error(std::format("Failed to create renderer: {}", SDL_GetError()));
        }

        return renderer;
    }

    /**
     * @brief Return the default flags for rendering the window
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Check, build and return the flags to apply the best configuration to render the window
     *
     * @return Uint32
     */
    Uint32 Window::getDefaultRenderFlags() const {
        int availableDrivers = SDL_GetNumRenderDrivers();
        SDL_RendererInfo info;
        Uint32 flags = 0;

        // Check all available drivers and prefer accelerated rendering with VSync
        for (int i = 0; i < availableDrivers; i++) {
            if (SDL_GetRenderDriverInfo(i, &info) == 0) {
                if (info.flags & SDL_RENDERER_ACCELERATED) {
                    flags |= SDL_RENDERER_ACCELERATED;
                }
                if (info.flags & SDL_RENDERER_PRESENTVSYNC) {
                    flags |= SDL_RENDERER_PRESENTVSYNC;
                }
                // Only use software rendering as fallback if no accelerated option exists
                if ((flags & SDL_RENDERER_ACCELERATED) == 0 && (info.flags & SDL_RENDERER_SOFTWARE)) {
                    flags |= SDL_RENDERER_SOFTWARE;
                }
            }
        }

        return flags;
    }

    void Window::addWindowFlag(SDL_WindowFlags flag)
    {
        this->flags->windowFlags = static_cast<SDL_WindowFlags>(this->flags->windowFlags | flag);
    }

    void Window::addRendererFlag(uint32_t flag)
    {
        this->flags->rendererFlags |= flag;
    }


    /**
     * @brief Add a flag to the current window or renderer flags
     *
     * Adds an SDL window flag to the existing window or renderer flags.
     *
     * @param uint8_t   target  Which kind of target will be applied to the flags
     * @param SDL_FLAGS flags   SDL window or renderer flag to add
     */
    void Window::addFlag(uint8_t target, SDL_FLAGS flags)
    {
        switch (target) {
            case Window::WINDOW_FLAGS:
                this->addWindowFlag(flags.windowFlags);
                break;
            case Window::RENDERER_FLAGS:
                this->addRendererFlag(flags.rendererFlags);
                break;
        }
    }

    /**
     * @brief Get the underlying SDL window pointer
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns the raw SDL_Window pointer for direct use with SDL and ImGui APIs.
     * This allows integration with SDL and ImGui rendering functions while keeping
     * window lifecycle management within the Window class.
     *
     * @return SDL_Window* Pointer to the SDL window, or nullptr if creation failed
     *
     * @note The returned pointer remains valid for the lifetime of this Window object
     * @note Do not manually destroy the returned SDL_Window - it's managed by this class
     */
    SDL_Window *Window::getWindow() const {
        return this->window;
    }

    /**
     * @brief Get the main display content scale factor
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns the DPI scaling factor that was determined during window creation.
     * This value is used to scale window dimensions for high-resolution displays
     * like Retina screens.
     *
     * @return float The content scale factor (1.0 for standard DPI, 2.0+ for high-DPI)
     *
     * @note This value is set once during construction and doesn't change
     */
    float Window::getMainScale() const {
        return this->mainScale;
    }

    /**
     * @brief Get the render handler for the current window
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Retuen the handler to render the window
     *
     * @return SDL_Renderer pointer
     *
     */
    SDL_Renderer *Window::getRenderer() const {
        return renderer;
    }

    /**
     * @brief Set the window rendering handler
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Set a valid handler for rendering the window
     *
     */
    void Window::setRenderer(SDL_Renderer *rendererHandler) {
        this->renderer = rendererHandler;
    }

    // void Window::addFlag(SDL_WindowFlags flag) const
    // {
    //     // this->flags != flag;
    // }
    //
    // SDL_WindowFlags Window::getFlag() const
    // {
    //     return this->flags;
    // }
}
