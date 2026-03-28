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

#ifndef ADS_SCENE_ENTITY_H
#define ADS_SCENE_ENTITY_H

#include "BaseEntity.h"
#include "Data/SceneData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Scene acts as the inspector adapter layer: it defines how scene
     * properties are presented in the inspector UI, validates incoming
     * values, and delegates all persistent storage to the backing
     * Data::SceneData struct owned by Core::Project.
     */
    class Scene : public BaseEntity {
    private:
        Data::SceneData* m_data; ///< Non-owning pointer to the backing SceneData

    public:
        /**
         * @brief Construct a new Scene backed by the given SceneData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param data Non-owning pointer to the SceneData struct. Must not be
         *             null and must outlive this entity.
         */
        explicit Scene(Data::SceneData* data);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Scene-specific getters/setters (operate on DataObject)
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        bool isStartScene() const;
        void setStartScene(bool isStart);

        const ADS::Types::Color& getBackgroundColor() const;
        void setBackgroundColor(const ADS::Types::Color& color);

        int getWidth() const;
        void setWidth(int width);

        int getHeight() const;
        void setHeight(int height);

        const std::string& getBackgroundImagePath() const;
        void setBackgroundImagePath(const std::string& path);

        const std::string& getMusicPath() const;
        void setMusicPath(const std::string& path);
    };
}

#endif //ADS_SCENE_ENTITY_H
