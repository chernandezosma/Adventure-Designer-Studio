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
#include <fstream>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <random>

#ifdef _WIN32
// Windows-specific headers
#else
#include <unistd.h>
#endif

#include "System.h"
#include "src/classes/env/env.h"
#include "i18nUtils.h"
#include "app.h"
#include "UI/Window.h"
#include "imgui/window_intialization_exception.h"

#ifdef BUILD_TESTING_ENABLED
#include "lib/googletest/googletest/include/gtest/internal/gtest-string.h"
#endif

#ifdef _WIN32
#include <windows.h>        // SetProcessDPIAware()
#endif

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "IDE/DesignTokens.h"
#include "languages.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace ADS::Constants; // ADS::Constants::System::SystemConst;

/**
 * @brief Application entry point
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * Initializes the Adventure Designer Studio application by setting up SDL,
 * creating the main window, loading fonts (including FontAwesome icons),
 * configuring ImGui backends, and running the main application loop.
 *
 * The function performs the following initialization steps:
 * 1. Creates the App instance and retrieves translation manager
 * 2. Creates main window with configured dimensions and position
 * 3. Loads default fonts and custom fonts from environment configuration
 * 4. Loads FontAwesome icon font for UI elements
 * 5. Sets up ImGui backends for SDL2 and SDL renderer
 * 6. Runs the application main loop
 * 7. Performs cleanup and shutdown
 *
 * @return 0 on successful execution
 *
 * @note This function handles all application lifecycle from initialization to shutdown
 * @see ADS::Core::App
 */
int main()
{
    try {
        auto *app = new ADS::Core::App();

        // Create window
        auto *sdlWindowInformation = new ADS::UI::SDL_WINDOW_INFO({
                app->getTranslationsManager()->_t("APP_TITLE"),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                System::DEFAULT_X_WIN_SIZE,
                System::DEFAULT_Y_WIN_SIZE,
        });

        auto *flags = new ADS::UI::SDL_FLAGS();
        ADS::UI::ImGuiManager &imguiObject = app->getImGuiObject();
        pair<boost::uuids::uuid, ADS::UI::Window *> windowInfo = imguiObject.newWindow(sdlWindowInformation, flags);
        ADS::UI::Window *mainWindow = windowInfo.second;
        app->setMainWindow(mainWindow);

        // Load fonts — must happen AFTER window creation so DPI scale is known.
        ADS::Environment *env = app->getEnv();
        ADS::UI::Fonts *fm = imguiObject.getFontManager();
        ADS::Core::App::setFontManager(fm);

        // OS-level display scale (e.g. 150 %, 200 % in desktop settings).
        // This is distinct from the physical pixel density that the ImGui
        // SDLRenderer3 backend detects and handles via SDL_SetRenderScale.
        float displayScale = SDL_GetWindowDisplayScale(mainWindow->getWindow());
        if (displayScale < 1.0f) displayScale = 1.0f;

        // Fallback: honour explicit DISPLAY_SCALE override from .env
        if (displayScale <= 1.0f) {
            std::string scaleOverride = env->getOrDefault("DISPLAY_SCALE", "");
            if (!scaleOverride.empty()) {
                try {
                    float v = std::stof(scaleOverride);
                    if (v > 0.0f) displayScale = v;
                } catch (...) {}
            }
        }
        spdlog::info("Font display scale: {:.2f}", displayScale);

        const float BASE_FONT = 16.0f;
        const float ICON_FONT = 13.0f;

        spdlog::info("Loading Fonts...");
        fm->loadFontFromFile("lightFont",   env->get("LIGHT_FONT")->data(),   BASE_FONT * displayScale);
        fm->loadFontFromFile("mediumFont",  env->get("MEDIUM_FONT")->data(),  BASE_FONT * displayScale);
        fm->loadFontFromFile("regularFont", env->get("REGULAR_FONT")->data(), BASE_FONT * displayScale);
        // Icons merge into the regular font — same scale
        fm->loadIconFont("public/fonts/FontAwesome/fontawesome-webfont.ttf",  ICON_FONT * displayScale);
        spdlog::info("Fonts loaded successfully");

        // Use the DPI-scaled TTF font as ImGui's global default
        imguiObject.getIO()->FontDefault = fm->getFont("regularFont");

        // Setup backends
        ImGui_ImplSDL3_InitForSDLRenderer(mainWindow->getWindow(), mainWindow->getRenderer());
        ImGui_ImplSDLRenderer3_Init(mainWindow->getRenderer());
        mainWindow->setStyle();

        // Load colour tokens — after setStyle() so overrides land on top of the theme
        ADS::IDE::Colors::loadFromFile("public/colors.ini");

        // Run the application
        spdlog::info("Run the application");
        app->run();

        // Cleanup
        spdlog::info("Shutdown the application");
        app->shutdown();
        delete app;

        return 0;
    } catch (const std::exception& e) {
        spdlog::error("Fatal error: {}", e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", e.what(), nullptr);
        return 1;
    } catch (...) {
        spdlog::error("Unknown fatal error occurred");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "Unknown error occurred", nullptr);
        return 1;
    }
}
