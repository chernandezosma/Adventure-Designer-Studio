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


#ifndef ADS_IMGUI_WINDOW_H
#define ADS_IMGUI_WINDOW_H
#include <SDL_render.h>
#include <SDL_video.h>
#include <ratio>
#include <string>

#include "imgui.h"

namespace ADS::UI {

    /**
     * Structure which hold the Window position, size and flags information
     */
    struct SDL_WINDOW_INFO
    {
        std::string title;
        int x;
        int y;
        int width;
        int height;
    };

    /**
     * Flags used to apply to Window and Renderer
     */
    struct SDL_FLAGS
    {
        SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(0);
        Uint32 rendererFlags = 0;
    };

    /**
     * DPI scale factors
     *
     * DPI Scale Reference:
     *  - 1.0 = Standard 1080p (96 DPI)
     *  - 1.5 = 1440p or some high DPI displays
     *  - 2.0 = 4K/Retina (192 DPI)
     *  - 3.0 = 6K displays
     */
    typedef struct
    {
        float diagonal;
        float horizontal;
        float vertical;
        float scale;
    } SDL_DPI;

    /**
     * Class to manage the Window
     */
    class Window
    {

    private:
        /**
         * Flags used for the current window, and its render
         */
        SDL_FLAGS *flags;

        /**
         * The Window handler itself
         */
        SDL_Window *window;

        /**
         * A structure representing rendering state
         */
        SDL_Renderer *renderer;

        /**
         * When viewports are enabled we tweak WindowRounding/WindowBg
         * so platform windows can look identical to regular ones.
         */
        ImGuiStyle *style;

        /**
         * The mainScale value ensures that the UI elements and fonts
         * are properly sized based on the display's DPI, making the
         * interface readable on both standard and high-resolution (4K,
         * retina, etc.) displays.
         *
         * This is a common pattern in ImGui applications to handle
         * DPI scaling correctly.
         */
        float mainScale;

        /**
         * Index of the display (monitor) where this window is currently located.
         * Retrieved via SDL_GetWindowDisplayIndex() and used to query display-specific
         * properties like DPI. Updates automatically if the window is moved to a
         * different monitor.
         */
        int displayIndex;

        /**
         * DPI (Dots Per Inch) information for the display where this window resides.
         * Contains diagonal, horizontal, and vertical DPI values, plus a computed
         * scale factor (diagonal DPI / 96.0). Used to properly scale UI elements
         * for high-resolution displays.
         *
         * @see SDL_DPI, setDPIScale()
         */
        SDL_DPI DPI;

    protected:
        /**
         * @brief Add a flag to the current window flags
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Adds an SDL window flag to the existing flags by performing a bitwise OR
         * operation. This allows combining multiple window flags to control window
         * behavior such as resizability, high DPI support, fullscreen mode, etc.
         *
         * @param flag SDL window flag to add
         *
         * @see addRendererFlag(), addFlag()
         */
        void addWindowFlag(SDL_WindowFlags flag);

        /**
         * @brief Add a flag to the current window renderer flags
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Adds an SDL renderer flag to the existing renderer flags by performing a
         * bitwise OR operation. This allows combining multiple renderer flags to control
         * rendering behavior such as VSync, hardware acceleration, software rendering, etc.
         *
         * @param flag SDL renderer flag to add
         *
         * @see addWindowFlag(), addFlag()
         */
        void addRendererFlag(uint32_t flag);

        /**
         * ImGui I/O handler reference
         */
        ImGuiIO *io;

    public:
        /**
         * Default flags
         */
        static constexpr SDL_WindowFlags DEFAULT_FLAGS = static_cast<SDL_WindowFlags>(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        static constexpr Uint32 DEFAULT_RENDER_FLAGS = static_cast<SDL_RendererFlags>(SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        static constexpr int FIRST_AVAILABLE_DRIVER = -1;
        static constexpr int WINDOW_FLAGS = 1;
        static constexpr int RENDERER_FLAGS = 2;
        static constexpr float DEFAULT_DPI_SCALE = 96.0f;

        /**
         * @brief Construct a new Window instance with SDL backend
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Creates an SDL window with the specified dimensions and title. Automatically
         * applies DPI scaling for high-resolution displays and combines provided flags
         * with default window flags.
         *
         * @param title     Window title displayed in the title bar
         * @param x         X position of the window (use SDL_WINDOWPOS_CENTERED for center)
         * @param y         Y position of the window (use SDL_WINDOWPOS_CENTERED for center)
         * @param width     Window width in logical units (scaled by DPI factor)
         * @param height    Window height in logical units (scaled by DPI factor)
         * @param flags     Additional SDL render and window flags (combined with existing ones)
         * @param io        Parameter to reference the ImGuiManager::io object
         *
         * @throws std::runtime_error if SDL window creation fails
         *
         * @see DEFAULT_FLAGS, getWindow()
         */
        Window(std::string title, int x, int y, int width, int height, SDL_FLAGS *flags, ImGuiIO *io);

        /**
         * @brief Construct a new Window from SDL_WINDOW_INFO structure
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Convenience constructor that accepts a SDL_WINDOW_INFO structure and
         * delegates to the main constructor. This simplifies window creation when
         * configuration is stored in a SDL_WINDOW_INFO struct.
         *
         * @param info     Pointer to SDL_WINDOW_INFO structure containing all window parameters
         * @param flags    Flags to be applied to the Window and Renderer
         * @param io       Parameter to reference the ImGuiManager::io object
         *
         * @throws std::runtime_error if SDL window creation fails
         *
         * @see internally uses Window::Window() constructor
         */
        Window(SDL_WINDOW_INFO *info, SDL_FLAGS *flags, ImGuiIO *io);

        /**
         * @brief Destructor for Window instance
         *
         * Default destructor that releases window resources. SDL cleanup should be
         * handled externally by the application.
         */
        ~Window() = default;

        /**
         * @brief Create a 2D rendering context for a window.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Create a 2D rendering context for a window.
         *
         * @param int index The index of the rendering driver to initialize, or -1 to
         *                  initialize the first one supporting the requested flags.
         * @param Uint32 rendererFlags  0, or one or more SDL_RendererFlags OR'd together.
         *
         * @throws std::runtime_error if SDL window creation fails (delegated from main constructor)
         *
         * @note This constructor uses constructor delegation (C++11 feature)
         * @see SDL_WINDOW_INFO, Window(std::string, float, float, float, float, SDL_WindowFlags)
         */
        SDL_Renderer *createRenderer(int index = -1) const;

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
        Uint32 getDefaultRenderFlags() const;

        /**
         * @brief Add a flag to the current window or renderer flags
         *
         * Adds an SDL window flag to the existing window or renderer flags.
         *
         * @param target    target to add the flag window or renderer
         * @param flags     SDL window flags to add
         */
        void addFlag(uint8_t target, SDL_FLAGS flags);

        /**
         * @brief Get the underlying SDL window pointer
         *
         * Returns the raw SDL_Window pointer for use with SDL and ImGui functions.
         * The window remains owned by this Window instance.
         *
         * @return SDL_Window* Pointer to the SDL window
         *
         * @note The returned pointer is valid only while this Window instance exists
         */
        SDL_Window *getWindow() const;

        /**
         * @brief Get the current window flags
         *
         * Returns the SDL window flags that were set during window creation,
         * including both default and custom flags.
         *
         * @return SDL_WindowFlags Current window flags
         */
        SDL_WindowFlags getFlags() const;

        /**
         * @brief Get the main display content scale factor
         *
         * Returns the content scale factor for the primary display, which is used
         * for high-DPI display support. This value is cached during initialization.
         *
         * @return float The content scale factor (typically 1.0 for standard DPI,
         *               2.0 for Retina/HiDPI displays)
         *
         * @note This value is set during init() and remains constant during runtime
         */
        [[nodiscard]] float getMainScale() const;

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
        [[nodiscard]] SDL_Renderer *getRenderer() const;

        /**
         * @brief Set the window rendering handler
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Set a valid handler for rendering the window
         *
         */
        void setRenderer(SDL_Renderer *rendererHandler);

        /**
         * @brief Query and store the DPI scaling information for the window's display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Retrieves the display index of the window and queries SDL for the diagonal,
         * horizontal, and vertical DPI values of that display. The DPI scale factor
         * is calculated as diagonal DPI / 96.0 (where 96 DPI is the standard baseline).
         * This is automatically called during window construction.
         *
         * @note If SDL_GetDisplayDPI fails, the scale defaults to 1.0
         * @see getDPIScale(), DEFAULT_DPI_SCALE
         */
        void setDPIScale();

        /**
         * @brief Get the complete DPI information structure
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the SDL_DPI structure containing all DPI-related values:
         * diagonal, horizontal, vertical DPI, and the computed scale factor.
         *
         * @return SDL_DPI Structure with diagonal, horizontal, vertical, and scale values
         *
         * @see setDPIScale(), SDL_DPI
         */
        SDL_DPI getDPIScale();

        /**
         * @brief Get the diagonal DPI of the window's display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the diagonal dots per inch measurement of the display where
         * the window is currently located. This is typically used to calculate
         * the overall DPI scaling factor.
         *
         * @return float Diagonal DPI value
         *
         * @see setDPIScale(), getHorizontalDpi(), getVerticalDpi()
         */
        float getDiagonalDpi();

        /**
         * @brief Get the horizontal DPI of the window's display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the horizontal dots per inch measurement of the display.
         * This can differ from diagonal DPI on displays with non-square pixels.
         *
         * @return float Horizontal DPI value
         *
         * @see setDPIScale(), getDiagonalDpi(), getVerticalDpi()
         */
        float getHorizontalDpi();

        /**
         * @brief Get the vertical DPI of the window's display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Returns the vertical dots per inch measurement of the display.
         * This can differ from diagonal DPI on displays with non-square pixels.
         *
         * @return float Vertical DPI value
         *
         * @see setDPIScale(), getDiagonalDpi(), getHorizontalDpi()
         */
        float getVerticalDpi();

        /**
         * @brief Configure ImGui style settings for viewports
         *
         * Adjusts window rounding and background opacity when multi-viewport
         * mode is enabled to ensure platform windows look consistent with
         * the main window.
         */
        void setStyle();


    };
}

#endif //_ADS_IMGUI_WINDOW_H
