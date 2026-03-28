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

#ifndef ADS_ITEM_ENTITY_H
#define ADS_ITEM_ENTITY_H

#include "BaseEntity.h"
#include "Data/ItemData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Item acts as the inspector adapter layer: it defines how item properties
     * are presented in the inspector UI, validates incoming values, and delegates
     * all persistent storage to the backing Data::ItemData struct owned by
     * Core::Project.
     */
    class Item : public BaseEntity {
    private:
        Data::ItemData* m_data; ///< Non-owning pointer to the backing ItemData

    public:
        /// Available item types
        static const std::vector<std::string>& getItemTypes();

        /**
         * @brief Construct a new Item backed by the given ItemData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the ItemData struct. Must not be
         *             null and must outlive this entity.
         */
        explicit Item(Data::ItemData* data);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Item-specific getters/setters (operate on DataObject)
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        bool isPickable() const;
        void setPickable(bool pickable);

        bool isUsable() const;
        void setUsable(bool usable);

        int getQuantity() const;
        void setQuantity(int quantity);

        int getItemType() const;
        void setItemType(int type);

        const std::string& getIconPath() const;
        void setIconPath(const std::string& path);

        const std::string& getStartingSceneId() const;
        void setStartingSceneId(const std::string& sceneId);

        /**
         * @brief Get the item type name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Human-readable type name
         */
        std::string getItemTypeName() const;
    };
}

#endif //ADS_ITEM_ENTITY_H
