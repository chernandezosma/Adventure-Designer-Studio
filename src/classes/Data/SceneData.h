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

#ifndef ADS_DATA_SCENE_DATA_H
#define ADS_DATA_SCENE_DATA_H

/**
 * @file SceneData.h
 * @brief Pure data class for a game scene — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include <string>
#include <vector>

#include "BaseData.h"
#include "Types/Color.h"

namespace ADS::Data {

    /**
     * @brief Exit connection from one scene to another.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Represents a directional exit in a scene. The direction is a
     * conventional string ("north", "south", "east", "west", "up", "down")
     * and targetSceneId references the destination scene by its unique id.
     */
    struct Exit
    {
        std::string direction; ///< Direction label (e.g. "north")
        std::string targetSceneId; ///< ID of the destination scene
    };

    /**
     * @brief Pure data class for a game scene.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Holds all persistent data for a scene. Only standard C++ types and
     * ADS::Types are used — no ImGui or UI framework types appear here.
     * This class is owned by Core::Project and is the authoritative
     * source of truth for the scene; Entities::Scene reads/writes through
     * a non-owning pointer to this object.
     */
    class SceneData : public BaseData
    {
        // Narrative description of the scene
        std::string m_description;

        // Whether this is the starting scene
        bool m_isStartScene = false;

        // Background display color
        ADS::Types::Color m_backgroundColor = {0.2f, 0.2f, 0.2f, 1.0f};

        // Scene width in pixels
        int m_width = 800;

        // Scene height in pixels
        int m_height = 600;

        // Path to background image asset
        std::string m_backgroundImagePath;

        // Path to background music asset
        std::string m_musicPath;

        // Directional exits to other scenes
        std::vector<Exit> m_exits;

        // IDs of items initially in this scene
        std::vector<std::string> m_presentItemIds;

        // IDs of characters initially in this scene
        std::vector<std::string> m_presentCharacterIds;

        bool m_fakeProperty;

    public:
        /**
         * @brief Get the narrative description of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Narrative description text
         */
        [[nodiscard]] const std::string &getDescription() const
        {
            return m_description;
        }

        /**
         * @brief Set the narrative description of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param description The new description text
         */
        void setDescription(const std::string &description)
        {
            m_description = description;
        }

        /**
         * @brief Check whether this is the game's starting scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the starting scene
         */
        [[nodiscard]] bool isStartScene() const
        {
            return m_isStartScene;
        }

        /**
         * @brief Set whether this is the game's starting scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param isStart True to mark this as the starting scene
         */
        void setStartScene(bool isStart)
        {
            m_isStartScene = isStart;
        }

        /**
         * @brief Get the background display color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Background color of the scene
         */
        [[nodiscard]] const ADS::Types::Color &getBackgroundColor() const
        {
            return m_backgroundColor;
        }

        /**
         * @brief Set the background display color
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param color The new background color
         */
        void setBackgroundColor(const ADS::Types::Color &color)
        {
            m_backgroundColor = color;
        }

        /**
         * @brief Get the scene width in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Scene width in pixels
         */
        [[nodiscard]] int getWidth() const
        {
            return m_width;
        }

        /**
         * @brief Set the scene width in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param width The new width value in pixels
         */
        void setWidth(int width)
        {
            m_width = width;
        }

        /**
         * @brief Get the scene height in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Scene height in pixels
         */
        [[nodiscard]] int getHeight() const
        {
            return m_height;
        }

        /**
         * @brief Set the scene height in pixels
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param height The new height value in pixels
         */
        void setHeight(int height)
        {
            m_height = height;
        }

        /**
         * @brief Get the path to the background image asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the background image
         */
        [[nodiscard]] const std::string &getBackgroundImagePath() const
        {
            return m_backgroundImagePath;
        }

        /**
         * @brief Set the path to the background image asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param path The new background image file path
         */
        void setBackgroundImagePath(const std::string &path)
        {
            m_backgroundImagePath = path;
        }

        /**
         * @brief Get the path to the background music asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& File path to the background music
         */
        [[nodiscard]] const std::string &getMusicPath() const
        {
            return m_musicPath;
        }

        /**
         * @brief Set the path to the background music asset
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param path The new background music file path
         */
        void setMusicPath(const std::string &path)
        {
            m_musicPath = path;
        }

        /**
         * @brief Get the list of directional exits from this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<Exit>& List of exits from this scene
         */
        [[nodiscard]] const std::vector<Exit> &getExits() const
        {
            return m_exits;
        }

        /**
         * @brief Set the list of directional exits for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param exits The new exits vector
         */
        void setExits(const std::vector<Exit> &exits)
        {
            m_exits = exits;
        }

        /**
         * @brief Get the IDs of items initially present in this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::string>& List of present item IDs
         */
        [[nodiscard]] const std::vector<std::string> &getPresentItemIds() const
        {
            return m_presentItemIds;
        }

        /**
         * @brief Set the IDs of items initially present in this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param ids The new item ID vector
         */
        void setPresentItemIds(const std::vector<std::string> &ids)
        {
            m_presentItemIds = ids;
        }

        /**
         * @brief Get the IDs of characters initially present in this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::vector<std::string>& List of present character IDs
         */
        [[nodiscard]] const std::vector<std::string> &getPresentCharacterIds() const
        {
            return m_presentCharacterIds;
        }

        /**
         * @brief Set the IDs of characters initially present in this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param ids The new character ID vector
         */
        void setPresentCharacterIds(const std::vector<std::string> &ids)
        {
            m_presentCharacterIds = ids;
        }

        void setFakeProperty(bool fakePropertyValue) const
        {
            m_fakeProperty = fakePropertyValue;
        }


        [[nodiscard]] bool getFakeProperty() const
        {
            return m_fakeProperty;
        }


    };

} // namespace ADS::Data

#endif // ADS_DATA_SCENE_DATA_H
