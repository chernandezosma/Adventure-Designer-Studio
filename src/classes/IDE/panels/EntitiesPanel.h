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


#ifndef ADS_ENTITIES_PANEL_H
#define ADS_ENTITIES_PANEL_H

#include "BasePanel.h"

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
    };
}

#endif //ADS_ENTITIES_PANEL_H
