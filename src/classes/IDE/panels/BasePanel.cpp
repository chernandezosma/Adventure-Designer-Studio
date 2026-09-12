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

/**
 * @file BasePanel.cpp
 * @brief Implementation of the BasePanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "BasePanel.h"

#include "imgui.h"

#include "IDE/DesignTokens.h"
#include "app.h"

namespace ADS::IDE::Panels {

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
    BasePanel::BasePanel(const std::string& name)
        : m_windowName(name), m_windowTitle(name), m_isVisible(true) {
        this->m_environment = Core::App::getEnv();
        this->m_translationsManager = Core::App::getTranslationsManager();
    }

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
    void BasePanel::setVisible(bool visible) {
        m_isVisible = visible;
    }

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
    bool BasePanel::isVisible() const {
        return m_isVisible;
    }

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
    const std::string& BasePanel::getWindowName() const {
        return m_windowName;
    }

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
    const i18n::i18n* BasePanel::getTranslationsManager() const {
        return this->m_translationsManager;
    };

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
    std::string BasePanel::getImGuiLabel() const {
        // Resolve the title every call so the dock tab follows a live UI
        // language switch. "###" keeps the window identity stable regardless
        // of the visible text.
        std::string title = m_windowTitle;
        if (!m_titleKey.empty() && m_translationsManager != nullptr) {
            title = m_translationsManager->_t(m_titleKey);
        }
        return title + "###" + m_windowName;
    }

    /**
     * @brief Open this panel's ImGui window with the shared caption styling
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param p_open Optional visibility flag ImGui toggles via the close box
     * @param flags  ImGuiWindowFlags (passed straight through)
     * @return bool ImGui::Begin's return
     */
    bool BasePanel::beginWindow(bool* p_open, int flags) {
        // Force light caption text while ImGui renders the (blue) title bar,
        // then restore the theme's normal text colour for the body.
        ImGui::PushStyleColor(ImGuiCol_Text, Colors::C_CAPTION_TEXT);
        const bool open = ImGui::Begin(getImGuiLabel().c_str(), p_open,
                                       static_cast<ImGuiWindowFlags>(flags));
        ImGui::PopStyleColor();
        return open;
    }

    /**
     * @brief Close a window opened with beginWindow()
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    void BasePanel::endWindow() {
        ImGui::End();
    }

}
