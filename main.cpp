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
#include <fstream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
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

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "languages.h"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

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
    SDL_SetMainReady();  // Required when using SDL_MAIN_HANDLED
    ADS::Core::App *app = new ADS::Core::App();

    // Create window
    ADS::UI::SDL_WINDOW_INFO *sdlWindowInformation = new ADS::UI::SDL_WINDOW_INFO({
            app->getTranslationsManager()->_t("WIN_TITLE"),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            System::DEFAULT_X_WIN_SIZE,
            System::DEFAULT_Y_WIN_SIZE,
    });

    ADS::UI::SDL_FLAGS *flags = new ADS::UI::SDL_FLAGS();
    ADS::UI::ImGuiManager &imguiObject = app->getImGuiObject();
    pair<boost::uuids::uuid, ADS::UI::Window *> windowInfo = imguiObject.newWindow(sdlWindowInformation, flags);
    ADS::UI::Window *mainWindow = windowInfo.second;
    app->setMainWindow(mainWindow);

    // Load fonts
    ADS::Environment *env = app->getEnv();
    ADS::UI::Fonts *fm = imguiObject.getFontManager();

    // Set the font manager as static member in App for global access
    ADS::Core::App::setFontManager(fm);

    fm->loadDefaultFonts();
    fm->loadFontFromFile("lightFont", env->get("LIGHT_FONT")->data());
    fm->loadFontFromFile("mediumFont", env->get("MEDIUM_FONT")->data());
    fm->loadFontFromFile("regularFont", env->get("REGULAR_FONT")->data());
    // Load icons AFTER other fonts so they merge with the regular font (which becomes default)
    fm->loadIconFont("public/fonts/FontAwesome/fontawesome-webfont.ttf", 13.0f);

    // Setup backends
    ImGui_ImplSDL2_InitForSDLRenderer(mainWindow->getWindow(), mainWindow->getRenderer());
    ImGui_ImplSDLRenderer2_Init(mainWindow->getRenderer());
    mainWindow->setStyle();

    // Run the application
    app->run();

    // Cleanup
    app->shutdown();
    delete app;

    return 0;
}
