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

#include <optional>
#include <string>
#include <vector>

#include "BaseEntity.h"
#include "Data/CharacterData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game character (player or NPC)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Character acts as the inspector adapter layer: it defines how character
     * properties are presented in the inspector UI, validates incoming values,
     * and delegates all persistent storage to the backing Data::CharacterData
     * struct owned by Core::Project. Property set follows
     * docs/core/schemas/character.md.
     */
    class Character : public BaseEntity
    {
    private:
        Data::CharacterData *m_data; ///< Non-owning pointer to the backing CharacterData

        /**
         * @brief Build the option-label list for every state in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Options provider for the "state" property. Empty if getProject() is
         * unset. Lists every state (not just "free" ones) — a Character's
         * "state" is a plain reference. Mirrors Entities::Scene and
         * Entities::Item.
         *
         * @return std::vector<std::string> One label per state, in project order
         */
        std::vector<std::string> buildStateOptionLabels() const;

        /**
         * @brief Build the option-label list for every scene in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Options provider for the "initial scene" dropdown. Empty if
         * getProject() is unset. Mirrors Entities::Scene / Entities::Item.
         *
         * @return std::vector<std::string> One label per scene, in project order
         */
        std::vector<std::string> buildSceneOptionLabels() const;

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
        explicit Character(Data::CharacterData *data);

        /**
         * @brief Get the typed character identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return ADS::Types::CharacterId The character's typed identifier
         */
        ADS::Types::CharacterId getCharacterId() const;

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Translated "Character" type name
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
         */
        std::vector<Inspector::PropertyDescriptor> getPropertyDescriptors() const override;

        /**
         * @brief Get the current value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param propertyId The unique property identifier string
         * @return Inspector::PropertyValue Current value, or std::monostate if unknown
         */
        Inspector::PropertyValue getPropertyValue(const std::string &propertyId) const override;

        /**
         * @brief Set the value of a property by ID
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param propertyId The unique property identifier string
         * @param value The new value (must match the property's expected type)
         * @return bool True if the value was accepted and written, false otherwise
         */
        bool setPropertyValue(const std::string &propertyId, const Inspector::PropertyValue &value) override;

        // Character-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the character's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::Descriptions& LexEngine text-id references
         */
        const Data::Descriptions &getDescriptions() const;

        /**
         * @brief Set the character's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Data::Descriptions &descriptions);

        /**
         * @brief Get the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::DescriptionTexts& Draft description text
         */
        const Data::DescriptionTexts &getDescriptionTexts() const;

        /**
         * @brief Set the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const Data::DescriptionTexts &texts);

        /**
         * @brief Check whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return bool True if this is the player-controlled character
         */
        bool isPlayer() const;

        /**
         * @brief Set whether this character is the player character
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param isPlayer True to mark this as the player character
         */
        void setPlayer(bool isPlayer);

        /**
         * @brief Get the character's capacities (load + life/stamina/sanity)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::Capacities& Current capacity values
         */
        const Data::Capacities &getCapacities() const;

        /**
         * @brief Set the character's capacities (load + life/stamina/sanity)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param capacities The new capacity values
         */
        void setCapacities(const Data::Capacities &capacities);

        /**
         * @brief Get the character's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
         */
        const std::optional<ADS::Types::StateId> &getState() const;

        /**
         * @brief Set the character's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param state The new state id, or std::nullopt to clear it
         */
        void setState(const std::optional<ADS::Types::StateId> &state);

        /**
         * @brief Get the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const ADS::Types::Color& Dialog text color
         */
        const ADS::Types::Color &getDialogColor() const;

        /**
         * @brief Set the color used for this character's dialog text
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event (as ImVec4) if the value actually changed.
         *
         * @param color The new dialog text color
         */
        void setDialogColor(const ADS::Types::Color &color);

        /**
         * @brief Get the path to the character's main (full-body) image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& File path to the main image
         */
        const std::string &getImagePath() const;

        /**
         * @brief Set the path to the character's main (full-body) image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new main image file path
         */
        void setImagePath(const std::string &path);

        /**
         * @brief Get the path to the character's avatar image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& File path to the avatar image
         */
        const std::string &getAvatarPath() const;

        /**
         * @brief Set the path to the character's avatar image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new avatar image file path
         */
        void setAvatarPath(const std::string &path);

        /**
         * @brief Get the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::optional<ADS::Types::SceneId>& Starting scene ID, or std::nullopt
         */
        const std::optional<ADS::Types::SceneId> &getInitialSceneId() const;

        /**
         * @brief Set the ID of the scene where this character starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param sceneId The new starting scene ID, or std::nullopt to clear it
         */
        void setInitialSceneId(const std::optional<ADS::Types::SceneId> &sceneId);

        /**
         * @brief Get the character's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return const std::vector<Data::Affordance>& The character's affordances
         */
        const std::vector<Data::Affordance>& getAffordances() const;

        /**
         * @brief Set the character's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param affordances The new affordance list
         */
        void setAffordances(const std::vector<Data::Affordance>& affordances);
    };
}

#endif //ADS_CHARACTER_ENTITY_H
