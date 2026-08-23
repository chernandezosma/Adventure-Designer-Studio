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
     *
     * Scalar fields (name, image, state, isStartScene, id) map to a single
     * Inspector property each. descriptions/exits/
     * items/triggers are exposed too, via PropertyType::Select for the
     * reference-style fields (exits, items, triggers) and one String
     * property per DescriptionTexts slot — see getPropertyDescriptors().
     * Exits/items option lists are resolved live from BaseEntity::getProject()
     * (falls back to an empty list if unset, e.g. in isolated unit tests).
     */
    class Scene : public BaseEntity {
    private:
        Data::SceneData* m_data; ///< Non-owning pointer to the backing SceneData

        /**
         * @brief Build the "(id) name" option-label list for every scene in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Used as the options provider for the ten exit direction
         * properties. Returns an empty list if getProject() is unset.
         *
         * @return std::vector<std::string> One label per scene, in project order
         */
        std::vector<std::string> buildSceneOptionLabels() const;

        /**
         * @brief Build the "(id) name" option-label list for every item in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Used as the options provider for the presentItemIds property.
         * Returns an empty list if getProject() is unset.
         *
         * @return std::vector<std::string> One label per item, in project order
         */
        std::vector<std::string> buildItemOptionLabels() const;

        /**
         * @brief Build the option-label list for every state in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Used as the options provider for the state property. Returns an
         * empty list if getProject() is unset. Lists every state (not just
         * "free" ones — see Project::getFreeStates()): a Scene's "state" is
         * a plain reference, so more than one scene may share the same one.
         *
         * @return std::vector<std::string> One label per state, in project order
         */
        std::vector<std::string> buildStateOptionLabels() const;

        /**
         * @brief Build the preset list of known affordance names
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * The example names from scene.md's Affordances definitions (Dark,
         * Lightable, Visitable), offered as a quick-add preset. Not an
         * enforced enum — an author may type any name.
         *
         * @return std::vector<std::string> Known affordance names
         */
        static std::vector<std::string> buildKnownAffordanceNames();

        /**
         * @brief Build the preset list of known affordance translation keys
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Index-paired with buildKnownAffordanceNames() — see
         * Data::Affordance::presetKey.
         *
         * @return std::vector<std::string> Known affordance translation keys
         */
        static std::vector<std::string> buildKnownAffordancePresetKeys();

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
         * category, for use by the inspector panel. Covers scalar fields
         * only — see the class-level docs for the complex-field deferral.
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
         * @brief Get the typed scene identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Distinct from BaseEntity::getId() (which returns a tag-erased
         * display string usable uniformly across entity kinds) — this
         * returns the underlying ADS::Types::SceneId for code (e.g.
         * Core::Project's CRUD, cross-entity references) that needs the
         * typed id.
         *
         * @return ADS::Types::SceneId The scene's typed identifier
         */
        ADS::Types::SceneId getSceneId() const;

        /**
         * @brief Get the sensory descriptions of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Not exposed as an Inspector property — see class-level docs.
         *
         * @return const Data::Descriptions& LexEngine text-id references for this scene
         */
        const Data::Descriptions& getDescriptions() const;

        /**
         * @brief Set the sensory descriptions of the scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Data::Descriptions& descriptions);

        /**
         * @brief Get the author-typed draft text for the scene's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const Data::DescriptionTexts& Draft description text
         */
        const Data::DescriptionTexts& getDescriptionTexts() const;

        /**
         * @brief Set the author-typed draft text for the scene's descriptions
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const Data::DescriptionTexts& texts);

        /**
         * @brief Get the scene's directional exits
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Not exposed as an Inspector property — see class-level docs.
         *
         * @return const Data::SceneData::Exits& The ten named directional exits
         */
        const Data::SceneData::Exits& getExits() const;

        /**
         * @brief Set the scene's directional exits
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param exits The new exits
         */
        void setExits(const Data::SceneData::Exits& exits);

        /**
         * @brief Get the IDs of items present in this scene at load time
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Not exposed as an Inspector property — see class-level docs.
         *
         * @return const std::vector<ADS::Types::ObjectId>& Present item IDs
         */
        const std::vector<ADS::Types::ObjectId>& getPresentItemIds() const;

        /**
         * @brief Set the IDs of items present in this scene at load time
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param ids The new present item ID vector
         */
        void setPresentItemIds(const std::vector<ADS::Types::ObjectId>& ids);

        /**
         * @brief Get the scene's trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Not exposed as an Inspector property — see class-level docs.
         *
         * @return const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Trigger map
         */
        const std::map<uint8_t, std::vector<ADS::Types::EventId>>& getTriggers() const;

        /**
         * @brief Replace the scene's entire trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param triggers The new trigger map
         */
        void setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>>& triggers);

        /**
         * @brief Append an EventId handler for a global trigger
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param eventId Global trigger id (e.g. 0x01 = on_enter)
         * @param handler EventId to append to that trigger's handler list
         */
        void addTrigger(uint8_t eventId, ADS::Types::EventId handler);

        /**
         * @brief Get the scene's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return const std::vector<Data::Affordance>& The scene's affordances
         */
        const std::vector<Data::Affordance>& getAffordances() const;

        /**
         * @brief Set the scene's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param affordances The new affordance list
         */
        void setAffordances(const std::vector<Data::Affordance>& affordances);

        /**
         * @brief Get the filename or base64-encoded image for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::string& Image filename or base64 payload
         */
        const std::string& getImage() const;

        /**
         * @brief Set the filename or base64-encoded image for this scene
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param image The new image filename or base64 payload
         */
        void setImage(const std::string& image);

        /**
         * @brief Get the scene's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Author-set reference to a state in the project's shared State
         * catalog.
         *
         * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
         */
        const std::optional<ADS::Types::StateId>& getState() const;

        /**
         * @brief Set the scene's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param state The new state id, or std::nullopt to clear it
         */
        void setState(const std::optional<ADS::Types::StateId>& state);

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
    };
}

#endif //ADS_SCENE_ENTITY_H