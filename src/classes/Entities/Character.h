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

#ifndef ADS_CHARACTER_ENTITY_H
#define ADS_CHARACTER_ENTITY_H

#include "BaseEntity.h"
#include "Data/CharacterData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game character (player or NPC)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Character acts as the inspector adapter layer: it defines how character
     * properties are presented in the inspector UI, validates incoming values,
     * and delegates all persistent storage to the backing Data::CharacterData
     * struct owned by Core::Project.
     */
    class Character : public BaseEntity {
    private:
        Data::CharacterData* m_data; ///< Non-owning pointer to the backing CharacterData

    public:
        /**
         * @brief Construct a new Character backed by the given CharacterData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the CharacterData struct. Must not be
         *             null and must outlive this entity.
         */
        explicit Character(Data::CharacterData* data);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Character-specific getters/setters (operate on DataObject)
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        int getHealth() const;
        void setHealth(int health);

        int getMaxHealth() const;
        void setMaxHealth(int maxHealth);

        bool isPlayer() const;
        void setPlayer(bool isPlayer);

        const ADS::Types::Color& getDialogColor() const;
        void setDialogColor(const ADS::Types::Color& color);

        const std::string& getPortraitPath() const;
        void setPortraitPath(const std::string& path);

        const std::string& getStartingSceneId() const;
        void setStartingSceneId(const std::string& sceneId);
    };
}

#endif //ADS_CHARACTER_ENTITY_H
