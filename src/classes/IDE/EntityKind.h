/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#ifndef ADS_IDE_ENTITY_KIND_H
#define ADS_IDE_ENTITY_KIND_H

/**
 * @file EntityKind.h
 * @brief The entity types the user can create / duplicate / delete
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 *
 * A deliberately small enum, distinct from @c Panels::NodeType (the tree's
 * richer node taxonomy, which also has sub-nodes, variables, audio, …). Only
 * the four kinds that have a full DataObject + CRUD + Inspector adapter and
 * are offered in the "Entities" menu and the tree context menu appear here.
 * Keeping it independent lets @c MenuBarRenderer expose entity commands
 * without depending on the panels layer.
 */

namespace ADS::IDE {

    /**
     * @brief One user-manageable entity type.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     */
    enum class EntityKind {
        Scene,      ///< ADS::Entities::Scene / ADS::Data::SceneData
        Character,  ///< ADS::Entities::Character / ADS::Data::CharacterData
        Item,       ///< ADS::Entities::Item / ADS::Data::ItemData
        State       ///< ADS::Entities::State / ADS::Data::StateData
    };

} // namespace ADS::IDE

#endif // ADS_IDE_ENTITY_KIND_H
