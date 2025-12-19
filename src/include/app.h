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


#ifndef ADS_APP_H
#define ADS_APP_H

#include "env/env.h"
#include <spdlog/spdlog.h>

#include "UI/UI.h"
#include "i18n/i18n.h"
#include "IDE/IDERenderer.h"

namespace ADS::Core {
    class App
    {
    private:
        /**
         * Environment class to manage the .env content.
         */
        Environment *environment;

        /**
         * Available translations for the app
         */
        i18n::i18n translations;

        /**
         * Imgui object to interact with the GUI
         */
        UI::ImGuiManager imguiObject;

        /**
         * Application running state flag.
         * True when the application is running, false when it should exit.
         */
        bool running;

        /**
         * Pointer to the main application window.
         * Used for event handling and rendering operations.
         */
        UI::Window *mainWindow;

        /**
         * SDL renderer for the main window.
         * Handles all rendering operations for the application.
         */
        SDL_Renderer *renderer;

        /**
         * IDE renderer for managing all IDE UI components.
         * Coordinates rendering of panels, layout, and menu bar.
         */
        IDE::IDERenderer *m_ideRenderer;

        /**
         * @brief Initialize all internal App structures and systems
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Dec 2025
         *
         * Performs initial setup of all App subsystems including logger initialization,
         * environment variable loading, translation system setup with configured languages,
         * and ImGui manager initialization. Sets running state to false and initializes
         * window/renderer pointers to nullptr.
         *
         * @note This is a private method called automatically by the constructor
         * @see App()
         */
        void init();

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
        void processEvents();

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
        void update();

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
        void render();

    public:
        App();
        ~App();

        /**
         * Return the environment for the whole App
         *
         * @return Environment
         */
        ADS::Environment *getEnv() const;

        /**
         * Delegate in Environment::isDebug function to return a boolean
         * indicating is debug mode is on or not.
         *
         * @return bool
         */
        bool isDebug();

        /**
         * Get the logger instance for app-wide usage
         *
         * @return Shared pointer to the spdlog logger
         */
        static std::shared_ptr<spdlog::logger> getLogger();

        /**
         * Get the translations instance for app-wide usage
         *
         * @return Reference to the i18n translations instance
         */
        i18n::i18n &getTranslations();

        /**
         * Return the translation for the text in language given
         *
         * @param text
         * @param language
         *
         * @return
         */
        string _t(const string &text, const string &language) const;

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
        [[nodiscard]] UI::ImGuiManager &getImGuiObject();

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
        void run();

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
        void shutdown();

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
        bool isRunning() const;

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
        void setMainWindow(UI::Window *window);
    };
}

#endif //ADS_APP_H
