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

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Always returns "Scene"
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Returns metadata for all editable scene properties, organised by
         * category, for use by the inspector panel.
         *
         * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
         */
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;

        /**
         * @brief Get the current value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param propertyId The unique property identifier string
         * @return Inspector::PropertyValue Current value, or std::monostate if unknown
         */
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;

        /**
         * @brief Set the value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Validates the type of @p value against the expected type for
         * @p propertyId before writing. Fires a property-changed event on success.
         *
         * @param propertyId The unique property identifier string
         * @param value The new value (must match the property's expected type)
         * @return bool True if the value was accepted and written, false otherwise
         */
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Scene-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the fake test property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool Current value of the fake property
         */
        bool getFakeProperty();

        /**
         * @brief Set the fake test property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param value New value for the fake property
         */
        void setFakeProperty(const bool& value);

        /**
         * @brief Get the narrative description of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Narrative description text
         */
        const std::string& getDescription() const;

        /**
         * @brief Set the narrative description of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param desc The new description text
         */
        void setDescription(const std::string& desc);

        /**
         * @brief Check whether this is the game's starting scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the starting scene
         */
        bool isStartScene() const;

        /**
         * @brief Set whether this is the game's starting scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param isStart True to mark this as the starting scene
         */
        void setStartScene(bool isStart);

        /**
         * @brief Get the background display color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Background color of the scene
         */
        const ADS::Types::Color& getBackgroundColor() const;

        /**
         * @brief Set the background display color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event (as ImVec4) if the value actually changed.
         *
         * @param color The new background color
         */
        void setBackgroundColor(const ADS::Types::Color& color);

        /**
         * @brief Get the scene width in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Scene width in pixels
         */
        int getWidth() const;

        /**
         * @brief Set the scene width in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param width The new width value in pixels
         */
        void setWidth(int width);

        /**
         * @brief Get the scene height in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Scene height in pixels
         */
        int getHeight() const;

        /**
         * @brief Set the scene height in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param height The new height value in pixels
         */
        void setHeight(int height);

        /**
         * @brief Get the path to the background image asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the background image
         */
        const std::string& getBackgroundImagePath() const;

        /**
         * @brief Set the path to the background image asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new background image file path
         */
        void setBackgroundImagePath(const std::string& path);

        /**
         * @brief Get the path to the background music asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the background music
         */
        const std::string& getMusicPath() const;

        /**
         * @brief Set the path to the background music asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new background music file path
         */
        void setMusicPath(const std::string& path);
    };
}

#endif //ADS_SCENE_ENTITY_H