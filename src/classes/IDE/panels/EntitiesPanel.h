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


#ifndef ADS_ENTITIES_PANEL_H
#define ADS_ENTITIES_PANEL_H

#include "BasePanel.h"
#include <functional>
#include "Inspector/IInspectable.h"
#include "Core/Project.h"


namespace ADS::IDE::Panels {
    /**
     * @brief Entities panel displaying the entity hierarchy
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a hierarchical tree view of game entities including scenes,
     * characters, and items. Provides functionality to add new entities.
     */
    class EntitiesPanel : public BasePanel {
    private:

        Inspector::IInspectable* m_selectedEntity = nullptr;
        Core::Project* m_project = nullptr;
        std::function<void(Inspector::IInspectable*)> m_onSelectionChanged;

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
        void renderSceneTree();

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
        void renderCharacterTree();

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
        void renderItemTree();

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
        void handleAddEntity();


    public:
        /**
         * @brief Construct a new EntitiesPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the entities panel with name "Entities".
         */
        EntitiesPanel();

        /**
         * @brief Destroy the EntitiesPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~EntitiesPanel() override = default;

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
        void render() override;

        /**
         * @brief Set the project data source for the entities panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Provides the panel with a non-owning pointer to the active project.
         * The panel will iterate the project's entity collections when rendering
         * the scene, character, and item trees.
         *
         * @param project Non-owning pointer to the project (may be nullptr to clear)
         */
        void setProject(Core::Project* project);

        /**
         * @brief Set the callback invoked when the user selects an entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * The callback receives a non-owning pointer to the selected entity.
         * IDERenderer uses this to forward selection events to InspectorPanel
         * without creating a direct dependency between the two panels.
         *
         * @param callback Function called with the newly selected IInspectable*
         */
        void setSelectionCallback(std::function<void(Inspector::IInspectable*)> callback);
    };
}

#endif //ADS_ENTITIES_PANEL_H
