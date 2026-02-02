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

#ifndef ADS_SCENE_ENTITY_H
#define ADS_SCENE_ENTITY_H

#include "BaseEntity.h"
#include "imgui.h"

namespace ADS::Entities {
    /**
     * @brief Scene entity representing a game location
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * A scene is a location in the game world where the player can
     * interact with characters, items, and other game elements.
     */
    class Scene : public BaseEntity {
    private:
        std::string m_description;      ///< Scene description text
        bool m_isStartScene;            ///< Whether this is the starting scene
        ImVec4 m_backgroundColor;       ///< Background color for the scene
        int m_width;                    ///< Scene width in pixels
        int m_height;                   ///< Scene height in pixels

    public:
        /**
         * @brief Construct a new Scene
         *
         * @param id Unique identifier
         * @param name Display name
         */
        Scene(const std::string& id, const std::string& name);

        // IInspectable interface
        std::string getTypeName() const override;
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Scene-specific getters/setters
        const std::string& getDescription() const;
        void setDescription(const std::string& desc);

        bool isStartScene() const;
        void setStartScene(bool isStart);

        const ImVec4& getBackgroundColor() const;
        void setBackgroundColor(const ImVec4& color);

        int getWidth() const;
        void setWidth(int width);

        int getHeight() const;
        void setHeight(int height);
    };
}

#endif //ADS_SCENE_ENTITY_H
