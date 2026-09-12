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


#ifndef ADS_BASE_PANEL_H
#define ADS_BASE_PANEL_H

#include <string>

#include "env/env.h"
#include "i18n/i18n.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Abstract base class for all IDE panels
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Provides common functionality for all IDE UI panels including
     * visibility management and window naming. All concrete panel classes
     * must implement the render() method.
     */
    class BasePanel
    {
    protected:
        /**
         * Translations manager
         */
        i18n::i18n *m_translationsManager;

        /**
         * Environment class to manage the .env content.
         */
        Environment *m_environment;

        /**
         * Stable window identifier used by ImGui for docking and window management
         */
        std::string m_windowName;

        /**
         * Translated display title shown in the window title bar. Used only
         * when m_titleKey is empty; otherwise getImGuiLabel() resolves the
         * title live from m_titleKey so it follows a language switch.
         */
        std::string m_windowTitle;

        /**
         * i18n key for the dock-tab / window title. Set this (instead of
         * assigning a translated m_windowTitle in the constructor) so the tab
         * re-localises on the fly when the UI language changes.
         */
        std::string m_titleKey;

        /**
         * Panel visibility state
         */
        bool m_isVisible;

    public:
        /**
         * @brief Construct a new BasePanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the base panel with the specified window name and sets
         * default visibility to true. Also initializes environment and translations
         * manager references from the application instance.
         *
         * @param name Window name for the panel
         */
        explicit BasePanel(const std::string &name);

        /**
         * @brief Destroy the BasePanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Virtual destructor to ensure proper cleanup of derived classes.
         * Default implementation is sufficient as this class uses only
         * non-owning pointers.
         */
        virtual ~BasePanel() = default;

        /**
         * @brief Render the panel (pure virtual method)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Must be implemented by derived classes to handle panel rendering.
         * This method is called every frame when the panel should be drawn.
         * Implementations should check visibility state and handle ImGui
         * rendering calls.
         */
        virtual void render() = 0;

        /**
         * @brief Set panel visibility
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Controls whether the panel should be rendered. When set to false,
         * the panel's render() method should return early without drawing.
         *
         * @param visible True to show panel, false to hide
         */
        void setVisible(bool visible);

        /**
         * @brief Get panel visibility state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the current visibility state of the panel. This does not
         * necessarily mean the panel is currently being displayed, only that
         * it is marked as visible.
         *
         * @return true if panel is visible
         * @return false if panel is hidden
         */
        bool isVisible() const;

        /**
         * @brief Get the window name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns the name used to identify this panel's window in ImGui.
         * This name is displayed in the window title bar and used internally
         * by ImGui for window management.
         *
         * @return const std::string& Window name reference
         */
        const std::string &getWindowName() const;

        /**
         * @brief Get the ImGui window label combining title and stable ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns a string in the form "Translated Title###stable_id" so that
         * ImGui uses the stable ID for docking/identity while displaying the
         * translated title in the title bar.
         *
         * @return std::string ImGui label with ### separator
         */
        std::string getImGuiLabel() const;

        /**
         * @brief Open this panel's ImGui window with the shared caption styling
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Wraps ImGui::Begin(getImGuiLabel(), …) and forces the caption-bar
         * text to Colors::C_CAPTION_TEXT (near-white) for the duration of the
         * title-bar render only — the blue caption fill needs light text in
         * both the dark and the light theme, and ImGui draws the title with
         * the plain ImGuiCol_Text. Pair every call with endWindow().
         *
         * @param p_open Optional visibility flag ImGui toggles via the close box
         * @param flags  ImGuiWindowFlags (passed straight through)
         * @return bool ImGui::Begin's return — false when the window is collapsed
         */
        bool beginWindow(bool* p_open = nullptr, int flags = 0);

        /**
         * @brief Close a window opened with beginWindow()
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Call once for every beginWindow(), regardless of its return value
         * (same contract as ImGui::Begin / ImGui::End).
         */
        void endWindow();

        /**
         * @brief Get the translations manager
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Returns a pointer to the i18n translations manager used for
         * localizing panel text and UI strings.
         *
         * @return const i18n::i18n* Pointer to the translations manager
         */
        const i18n::i18n* getTranslationsManager() const;
    };
}

#endif //ADS_BASE_PANEL_H
