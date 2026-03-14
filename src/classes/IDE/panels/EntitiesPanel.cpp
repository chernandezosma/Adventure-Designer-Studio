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
        : BasePanel("hEntities") {
        m_windowTitle = this->getTranslationsManager()->_t("ENTITIES");
    }

    /**
     * @brief Render the scenes tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Displays a collapsible tree node containing all scene entities
     * from the active project. Each scene is rendered as a selectable row;
     * clicking one updates m_selectedEntity and fires m_onSelectionChanged.
     *
     * @note Returns immediately if no project has been set via setProject()
     * @see setProject(), setSelectionCallback()
     */
    void EntitiesPanel::renderSceneTree() {
        if (!m_project) return;
        if (ImGui::TreeNode(this->getTranslationsManager()->_t("TREE_NODE_SCENE").c_str())) {
            for (const auto& scene : m_project->getScenes()) {
                bool selected = (m_selectedEntity == scene.get());
                if (ImGui::Selectable(scene->getDisplayName().c_str(), selected)) {
                    m_selectedEntity = scene.get();
                    if (m_onSelectionChanged) m_onSelectionChanged(m_selectedEntity);
                }
            }
            ImGui::TreePop();
        }
    }

    /**
     * @brief Render the characters tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Displays a collapsible tree node containing all character entities
     * from the active project. Each character is rendered as a selectable row;
     * clicking one updates m_selectedEntity and fires m_onSelectionChanged.
     *
     * @note Returns immediately if no project has been set via setProject()
     * @see setProject(), setSelectionCallback()
     */
    void EntitiesPanel::renderCharacterTree() {
        if (!m_project) return;
        if (ImGui::TreeNode(this->getTranslationsManager()->_t("TREE_NODE_CHARACTERS").c_str())) {
            for (const auto& character : m_project->getCharacters()) {
                bool selected = (m_selectedEntity == character.get());
                if (ImGui::Selectable(character->getDisplayName().c_str(), selected)) {
                    m_selectedEntity = character.get();
                    if (m_onSelectionChanged) m_onSelectionChanged(m_selectedEntity);
                }
            }
            ImGui::TreePop();
        }
    }

    /**
     * @brief Render the items tree node
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Displays a collapsible tree node containing all item entities
     * from the active project. Each item is rendered as a selectable row;
     * clicking one updates m_selectedEntity and fires m_onSelectionChanged.
     *
     * @note Returns immediately if no project has been set via setProject()
     * @see setProject(), setSelectionCallback()
     */
    void EntitiesPanel::renderItemTree() {
        if (!m_project) return;
        if (ImGui::TreeNode(this->getTranslationsManager()->_t("TREE_NODE_ITEMS").c_str())) {
            for (const auto& item : m_project->getItems()) {
                bool selected = (m_selectedEntity == item.get());
                if (ImGui::Selectable(item->getDisplayName().c_str(), selected)) {
                    m_selectedEntity = item.get();
                    if (m_onSelectionChanged) m_onSelectionChanged(m_selectedEntity);
                }
            }
            ImGui::TreePop();
        }
    }

    /**
     * @brief Set the project data source for the entities panel
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Provides the panel with a non-owning pointer to the active project.
     * The panel will iterate the project's entity collections when rendering
     * the scene, character, and item trees. Passing nullptr clears the data
     * source and resets the current selection.
     *
     * @param project Non-owning pointer to the project (may be nullptr to clear)
     * @see renderSceneTree(), renderCharacterTree(), renderItemTree()
     */
    void EntitiesPanel::setProject(Core::Project* project) {
        m_project = project;
        m_selectedEntity = nullptr;
    }

    /**
     * @brief Set the callback invoked when the user selects an entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * The callback receives a non-owning pointer to the selected IInspectable.
     * IDERenderer uses this to forward selection events to InspectorPanel
     * without creating a direct dependency between the two panels.
     * Passing an empty function clears any previously registered callback.
     *
     * @param callback Function called with the newly selected IInspectable*
     */
    void EntitiesPanel::setSelectionCallback(std::function<void(Inspector::IInspectable*)> callback) {
        m_onSelectionChanged = std::move(callback);
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

        ImGui::Begin(getImGuiLabel().c_str());

        ImGui::Text("%s", this->getTranslationsManager()->_t("ENTITIES_LIST").c_str());
        ImGui::Separator();

        renderSceneTree();
        renderCharacterTree();
        renderItemTree();

        ImGui::Separator();
        if (ImGui::Button(this->getTranslationsManager()->_t("CLICK_TO_ADD_NEW_SCRIPT").c_str())) {
            handleAddEntity();
        }

        ImGui::End();
    }
}
