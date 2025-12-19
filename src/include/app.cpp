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


#include "app.h"

#include <SDL.h>
#include "adsString.h"
#include "i18nUtils.h"
#include "Logger/logger.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "System.h"

// Forward declaration of RenderIDE from main.cpp
extern void RenderIDE();

namespace ADS::Core {
    void App::init()
    {
        Logger::init();
        spdlog::info(format("{}:{} - Retrieve the languages from .env file", __FILE__, __LINE__));
        const std::string *languagesAllowedFromEnv = this->environment->get("LANGUAGES");
        spdlog::info("Load the languages");

        for (const std::vector<std::string> languages = explode(*languagesAllowedFromEnv, ',');
             const string &language: languages) {
            this->translations.addLanguage(std::string(language));
        }
        spdlog::info(format("{}:{} - Using im GUI Library", __FILE__, __LINE__));
        this->imguiObject = UI::ImGuiManager();

        // Initialize application state
        this->running = false;
        this->mainWindow = nullptr;
        this->renderer = nullptr;
    }

    /**
     * @brief Construct App instance and initialize environment
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Creates a new App instance and allocates an Environment object
     * to manage application configuration from .env files and initializes
     * the translation system.
     */
    App::App() :
        translations(
                "public/translations/core",
                std::string(ADS::Constants::Languages::ENGLISH_UNITED_STATES)
                )
    {
        this->environment = new ADS::Environment();
        this->init();
    }

    /**
     * @brief Destroy App instance and release resources
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Deallocates the Environment object to prevent memory leaks.
     */
    App::~App()
    {
        delete this->environment;
    }

    /**
     * @brief Retrieve the application environment manager
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the internal Environment object for reading
     * configuration values from .env files.
     *
     * @return Pointer to the ADS::Environment instance
     *
     * @warning The returned pointer is only valid while the App
     *          object exists
     */
    ADS::Environment *App::getEnv() const
    {
        return this->environment;
    }

    /**
     * @brief Get the logger instance for app-wide usage
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the initialized logger that can be used
     * throughout the application. Allows other components to use
     * the logger without direct dependency on the Logger class.
     *
     * @return Shared pointer to the spdlog logger instance
     *
     * @see ADS::Core::Logger::getInstance()
     */
    std::shared_ptr<spdlog::logger> App::getLogger()
    {
        return ADS::Core::Logger::getInstance();
    }

    /**
     * @brief Get the translations instance for app-wide usage
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Provides access to the initialized translation system that can be
     * used throughout the application for localized text.
     *
     * @return Reference to the i18n translations instance
     *
     * @see i18n::i18n
     */
    i18n::i18n &App::getTranslations()
    {
        return this->translations;
    }

    /**
     * @brief Return the translations for the text on selected language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * @param text          Key to translate. It must exist in translations
     * @param language      Language used to get the translation
     *
     * @return Reference to the i18n translations instance
     *
     * @see i18n::i18n
     */
    string App::_t(const string &text, const string &language) const
    {
        return this->translations.translate(text, language);
    }

    /**
     * @brief Check if application is running in debug mode
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Nov 2025
     *
     * Delegates to the Environment object to determine if the DEBUG
     * variable is enabled in the configuration.
     *
     * @return true if DEBUG is enabled in the environment
     * @return false if DEBUG is disabled or not set
     *
     * @see ADS::Environment::isDebug()
     */
    bool App::isDebug()
    {
        return this->environment->isDebug();
    }

    /**
     * @brief Get the ImGui manager instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns a reference to the ImGuiManager instance that handles
     * all ImGui context, configuration, and UI management operations.
     *
     * @return UI::ImGuiManager& Reference to the ImGui manager instance
     *
     * @note The [[nodiscard]] attribute indicates the return value should not be ignored
     */
    [[nodiscard]] UI::ImGuiManager &App::getImGuiObject()
    {
        return this->imguiObject;
    }

    /**
     * @brief Execute the main application loop
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Runs the main application loop which continuously processes events,
     * updates application state, and renders frames until the application
     * is signaled to exit. This is a blocking call that only returns when
     * the application should terminate.
     *
     * The main loop executes three phases per iteration:
     * 1. Event processing (user input, window events)
     * 2. State updates (game logic, animations)
     * 3. Rendering (UI and graphics)
     *
     * @note Must be called after proper initialization of window and ImGui backends
     * @see processEvents(), update(), render(), isRunning()
     */
    void App::run()
    {
        running = true;

        while (running) {
            processEvents();
            update();
            render();
        }
    }

    /**
     * @brief Process SDL events and user input
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Polls and processes all pending SDL events including window events,
     * user input, and system messages. Forwards ImGui-relevant events to
     * the ImGui backend and handles quit requests.
     *
     * @note Sets running to false on SDL_QUIT or window close events
     * @see run(), isRunning()
     */
    void App::processEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(mainWindow->getWindow()))
                running = false;
        }
    }

    /**
     * @brief Update application state and logic
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Called once per frame to update application state, game logic,
     * animations, and other time-dependent operations. Currently reserved
     * for future implementation of application logic updates.
     *
     * @see run(), render()
     */
    void App::update()
    {
        // Future: game logic, state updates, animations, etc.
    }

    /**
     * @brief Render the current frame to the screen
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Prepares and renders a complete frame including ImGui UI elements.
     * Handles frame preparation, UI rendering via RenderIDE(), and final
     * presentation to the screen. Supports multi-viewport rendering when
     * enabled in ImGui configuration.
     *
     * @note Automatically handles DPI scaling and platform-specific rendering
     * @see run(), update(), RenderIDE()
     */
    void App::render()
    {
        ImGuiIO *io = imguiObject.getIO();

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render the IDE
        RenderIDE();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 45, 45, 48, 255); // Dark gray background
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        // Update and Render additional Platform Windows
        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        SDL_RenderPresent(renderer);
    }

    /**
     * @brief Perform cleanup and shutdown of all application systems
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Safely shuts down and cleans up all application resources including:
     * - Saving ImGui configuration to disk
     * - Shutting down ImGui backends (SDL2 and SDLRenderer2)
     * - Destroying ImGui context
     * - Releasing SDL renderer and window resources
     * - Quitting SDL subsystems
     *
     * Should be called after run() exits and before application termination.
     *
     * @note Must be called before deleting the App instance
     * @see run()
     */
    void App::shutdown()
    {
        using namespace ADS::Constants;

        ImGui::SaveIniSettingsToDisk(System::CONFIG_FILE);
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(mainWindow->getWindow());
        SDL_Quit();

        spdlog::info("Application shutdown complete");
    }

    /**
     * @brief Check if the application is currently running
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Returns the current running state of the application. The application
     * is considered running from the start of run() until a quit event is
     * received or shutdown is requested.
     *
     * @return true if the application is running
     * @return false if the application should exit
     *
     * @see run(), processEvents()
     */
    bool App::isRunning() const
    {
        return this->running;
    }

    /**
     * @brief Set the main application window
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Assigns the main window for the application and caches its renderer
     * for use during the main loop. This must be called before run() is
     * invoked to ensure proper event handling and rendering.
     *
     * @param window Pointer to the Window instance to use as the main window
     *
     * @note The window's renderer is automatically extracted and cached
     * @see run(), processEvents(), render()
     */
    void App::setMainWindow(UI::Window *window)
    {
        this->mainWindow = window;
        this->renderer = window->getRenderer();
        spdlog::info("Main window set successfully");
    }
} // ADS
