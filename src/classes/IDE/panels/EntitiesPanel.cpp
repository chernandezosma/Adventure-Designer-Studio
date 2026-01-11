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

/**
 * @file EntitiesPanel.cpp
 * @brief Implementation of the EntitiesPanel class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "EntitiesPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Construct a new EntitiesPanel object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the entities panel with name "Entities".
     */
    EntitiesPanel::EntitiesPanel()
        : BasePanel("Entities") {
    }

    /**
     * @brief Render the scenes tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a collapsible tree node containing all scene entities
     * in the current project. Each scene is displayed as a bullet item.
     *
     * @note Currently displays placeholder data
     */
    void EntitiesPanel::renderSceneTree() {
        if (ImGui::TreeNode("Scenes")) {
            ImGui::BulletText("Scene 1");
            ImGui::BulletText("Scene 2");
            ImGui::BulletText("Scene 3");
            ImGui::TreePop();
        }
    }

    /**
     * @brief Render the characters tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a collapsible tree node containing all character entities
     * in the current project. Each character is displayed as a bullet item.
     *
     * @note Currently displays placeholder data
     */
    void EntitiesPanel::renderCharacterTree() {
        if (ImGui::TreeNode("Characters")) {
            ImGui::BulletText("Hero");
            ImGui::BulletText("Villain");
            ImGui::BulletText("NPC 1");
            ImGui::TreePop();
        }
    }

    /**
     * @brief Render the items tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays a collapsible tree node containing all item entities
     * in the current project. Each item is displayed as a bullet item.
     *
     * @note Currently displays placeholder data
     */
    void EntitiesPanel::renderItemTree() {
        if (ImGui::TreeNode("Items")) {
            ImGui::BulletText("Sword");
            ImGui::BulletText("Key");
            ImGui::BulletText("Potion");
            ImGui::TreePop();
        }
    }

    /**
     * @brief Handle add entity button click
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Handles the user action when the "Add New Entity" button is clicked.
     * This method will be responsible for creating new entities in the project.
     *
     * @note Currently a placeholder implementation
     */
    void EntitiesPanel::handleAddEntity() {
        // Handle add entity
    }

    /**
     * @brief Render the entities panel
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Displays the entity hierarchy tree with scenes, characters,
     * and items, along with an add entity button. The panel provides
     * a structured view of all game entities organized by type.
     *
     * @note Returns early if panel is not visible
     * @see renderSceneTree(), renderCharacterTree(), renderItemTree()
     */
    void EntitiesPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(m_windowName.c_str());

        ImGui::Text("Entities List");
        ImGui::Separator();

        renderSceneTree();
        renderCharacterTree();
        renderItemTree();

        ImGui::Separator();
        if (ImGui::Button("Add New Entity")) {
            handleAddEntity();
        }

        ImGui::End();
    }
}
