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

#ifndef ADS_DATA_IIDENTIFIABLE_H
#define ADS_DATA_IIDENTIFIABLE_H

/**
 * @file IIdentifiable.h
 * @brief Tag-erased id/name interface implemented by BaseData<Tag>
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>

namespace ADS::Data {

    /**
     * @brief Non-template interface exposing id/name for any BaseData<Tag>
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * BaseData is a template family (one instantiation per entity tag), so
     * it cannot be referenced by a single non-template pointer type. This
     * interface gives Entities::BaseEntity a uniform, tag-erased way to
     * read the id (as a display string) and read/write the name across
     * Scene/Item/Character DataObjects.
     */
    class IIdentifiable {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~IIdentifiable() = default;

        /**
         * @brief Get the unique entity identifier as a display string
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Stringified form of the underlying typed id
         */
        [[nodiscard]] virtual std::string getIdString() const = 0;

        /**
         * @brief Get the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Display name of this entity
         */
        [[nodiscard]] virtual const std::string& getName() const = 0;

        /**
         * @brief Set the human-readable display name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name The new display name string
         */
        virtual void setName(const std::string& name) = 0;
    };

} // namespace ADS::Data

#endif // ADS_DATA_IIDENTIFIABLE_H