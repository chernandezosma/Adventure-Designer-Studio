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

#ifndef ADS_NFD_WINDOW_HANDLE_H
#define ADS_NFD_WINDOW_HANDLE_H

/**
 * @file NfdWindowHandle.h
 * @brief SDL3 → NFD native window handle bridge
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * nativefiledialog-extended's own SDL glue (nfd_sdl2.h) is SDL2-only
 * (built on SDL_GetWindowWMInfo/SDL_SysWMinfo, which don't exist in
 * SDL3). This header fills that gap using SDL3's window-properties API
 * so a dialog can be parented to the app's main window instead of
 * appearing as an unrelated top-level OS window.
 */

#include <nfd.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>

namespace ADS::UI {

    /**
     * @brief Resolve the native window handle NFD needs to parent a dialog
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Only the X11 branch is exercised/verified in this project's Linux
     * dev environment. The Windows/macOS branches use the equivalent
     * documented SDL3 window-property keys but are otherwise untested here.
     * Returns an unset handle (today's behavior — NFD simply doesn't
     * parent the dialog) for a null window or when the platform/session
     * doesn't expose a usable handle (e.g. Wayland, which NFD 1.2.1 has
     * no handle type for at all).
     *
     * @param window The SDL window to parent dialogs to, or nullptr
     * @return nfdwindowhandle_t Handle to pass as NFD::OpenDialog/SaveDialog's parentWindow argument
     */
    inline nfdwindowhandle_t getNfdParentWindowHandle(SDL_Window* window) {
        nfdwindowhandle_t handle{};
        handle.type = NFD_WINDOW_HANDLE_TYPE_UNSET;
        handle.handle = nullptr;

        if (!window) {
            return handle;
        }

        SDL_PropertiesID props = SDL_GetWindowProperties(window);
        if (props == 0) {
            return handle;
        }

#if defined(_WIN32)
        void* hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        if (hwnd) {
            handle.type = NFD_WINDOW_HANDLE_TYPE_WINDOWS;
            handle.handle = hwnd;
        }
#elif defined(__APPLE__)
        void* nsWindow = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        if (nsWindow) {
            handle.type = NFD_WINDOW_HANDLE_TYPE_COCOA;
            handle.handle = nsWindow;
        }
#else
        Sint64 x11Window = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (x11Window != 0) {
            handle.type = NFD_WINDOW_HANDLE_TYPE_X11;
            handle.handle = reinterpret_cast<void*>(static_cast<intptr_t>(x11Window));
        }
#endif

        return handle;
    }

} // namespace ADS::UI

#endif // ADS_NFD_WINDOW_HANDLE_H
