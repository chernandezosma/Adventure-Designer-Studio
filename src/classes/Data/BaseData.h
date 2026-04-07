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

namespace ADS::Data {

    /**
     * @brief Common base class for all DataObject types.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Holds the two fields shared by every game entity: a unique
     * identifier and a human-readable display name. All concrete
     * DataObjects (SceneData, CharacterData, ItemData) inherit from
     * this class. No ImGui or rendering types appear here.
     */
    class BaseData {
    public:
        /**
         * @brief Get the unique entity identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Unique identifier of this entity
         */
        [[nodiscard]] const std::string& getId() const { return m_id; }

        /**
         * @brief Set the unique entity identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param id The new identifier string
         */
        void setId(const std::string& id) { m_id = id; }

        /**
         * @brief Get the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Display name of this entity
         */
        [[nodiscard]] const std::string& getName() const { return m_name; }

        /**
         * @brief Set the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name The new display name string
         */
        void setName(const std::string& name) { m_name = name; }

    private:
        std::string m_id;   ///< Unique entity identifier
        std::string m_name; ///< Human-readable display name
    };

} // namespace ADS::Data

#endif // ADS_DATA_BASE_DATA_H
