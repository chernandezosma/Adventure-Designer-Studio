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
     * @version Dec 2025
     *
     * Renders a hierarchical tree view of game entities including scenes,
     * characters, and items. Provides functionality to add new entities.
     */
    class EntitiesPanel : public BasePanel {
    private:
        /**
         * @brief Render the scenes tree node
         */
        void renderSceneTree();

        /**
         * @brief Render the characters tree node
         */
        void renderCharacterTree();

        /**
         * @brief Render the items tree node
         */
        void renderItemTree();

        /**
         * @brief Handle add entity button click
         */
        void handleAddEntity();

    public:
        /**
         * @brief Construct a new EntitiesPanel object
         */
        EntitiesPanel();

        /**
         * @brief Destroy the EntitiesPanel object
         */
        ~EntitiesPanel() override = default;

        /**
         * @brief Render the entities panel
         *
         * Displays the entity hierarchy tree with scenes, characters,
         * and items, along with an add entity button.
         */
        void render() override;
    };
}

#endif //ADS_ENTITIES_PANEL_H
