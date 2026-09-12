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

#ifndef ADS_DATA_BASE_DATA_H
#define ADS_DATA_BASE_DATA_H

/**
 * @file BaseData.h
 * @brief Common base for all game DataObjects (id + name)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>

#include "IIdentifiable.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Common base class template for all DataObject types.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @tparam Tag Phantom tag identifying the concrete entity kind (e.g.
     *             ADS::Types::SceneTag), so that a SceneData's id cannot be
     *             confused with an ItemData's or CharacterData's at compile
     *             time. See ADS::Types::Id.
     *
     * Holds the two fields shared by every game entity: a unique typed
     * identifier and a human-readable display name. Concrete DataObjects
     * inherit a specific instantiation: SceneData : BaseData<SceneTag>,
     * ItemData : BaseData<ObjectTag>, CharacterData : BaseData<CharacterTag>.
     * Implements IIdentifiable so that Entities::BaseEntity can hold a
     * single non-template pointer across all three. No ImGui or rendering
     * types appear here.
     */
    template<typename Tag>
    class BaseData : public IIdentifiable {
    public:
        /**
         * @brief Get the unique entity identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Id<Tag>& Unique identifier of this entity
         */
        [[nodiscard]] const ADS::Types::Id<Tag>& getId() const { return m_id; }

        /**
         * @brief Set the unique entity identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id The new identifier
         */
        void setId(const ADS::Types::Id<Tag>& id) { m_id = id; }

        /**
         * @brief Get the unique entity identifier as a display string
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Stringified form of the typed id
         */
        [[nodiscard]] std::string getIdString() const override {
            return std::to_string(m_id.value);
        }

        /**
         * @brief Get the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Display name of this entity
         */
        [[nodiscard]] const std::string& getName() const override { return m_name; }

        /**
         * @brief Set the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name The new display name string
         */
        void setName(const std::string& name) override { m_name = name; }

    private:
        ADS::Types::Id<Tag> m_id{0}; ///< Unique entity identifier — 0 = unassigned
        std::string m_name;          ///< Human-readable display name
    };

} // namespace ADS::Data

#endif // ADS_DATA_BASE_DATA_H