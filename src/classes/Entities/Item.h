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

#include <optional>
#include <string>
#include <vector>

#include "BaseEntity.h"
#include "Data/ItemData.h"

namespace ADS::Entities {
    /**
     * @brief Inspector adapter for a game item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Item acts as the inspector adapter layer: it defines how item properties
     * are presented in the inspector UI, validates incoming values, and delegates
     * all persistent storage to the backing Data::ItemData struct owned by
     * Core::Project. Property set follows docs/core/schemas/item.md.
     */
    class Item : public BaseEntity {
    private:
        Data::ItemData* m_data; ///< Non-owning pointer to the backing ItemData

        /**
         * @brief Build the option-label list for every state in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
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
         * getProject() is unset. Mirrors Entities::Scene.
         *
         * @return std::vector<std::string> One label per scene, in project order
         */
        std::vector<std::string> buildSceneOptionLabels() const;

        /**
         * @brief Build the option-label list for every item in the project
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Used by the "container" and "combinable with" multi-select fields.
         * Mirrors Entities::Scene::buildItemOptionLabels(). The item being
         * edited is omitted — an item can neither contain nor combine with
         * itself. One label per entry of relationItemOptions(), same order.
         *
         * @return std::vector<std::string> One label per selectable item
         */
        std::vector<std::string> buildItemOptionLabels() const;

        /**
         * @brief Project items eligible for the container / combinable lists.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Every item in the project except this one, in project order. The
         * single source of truth for the option ↔ id mapping of the
         * "containerItems" and "combinableWith" properties, so
         * buildItemOptionLabels(), getPropertyValue() and setPropertyValue()
         * all stay in step.
         *
         * @return std::vector<Item*> Non-owning pointers, self excluded
         */
        std::vector<Item*> relationItemOptions() const;

        /**
         * @brief Build the preset list of known affordance names
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * The names from item.md's former fixed bitmap (Takeable,
         * Droppable, …), offered as a quick-add preset in the affordances
         * editor. Not an enforced enum — an author may type any name.
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
         * @brief Get the list of available item type names
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::vector<std::string> Available item type names
         */
        static std::vector<std::string> getItemTypes();

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

        /**
         * @brief Get the typed item identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return ADS::Types::ObjectId The item's typed identifier
         */
        ADS::Types::ObjectId getItemId() const;

        // IInspectable interface

        /**
         * @brief Get the type name of this entity
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Translated "Item" type name
         */
        std::string getTypeName() const override;

        /**
         * @brief Get the list of property descriptors for this item
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
        Inspector::PropertyValue getPropertyValue(const std::string& propertyId) const override;

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
        bool setPropertyValue(
            const std::string& propertyId,
            const Inspector::PropertyValue& value
        ) override;

        // Item-specific getters/setters (operate on DataObject)

        /**
         * @brief Get the item's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::Descriptions& LexEngine text-id references
         */
        const Data::Descriptions& getDescriptions() const;

        /**
         * @brief Set the item's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Data::Descriptions& descriptions);

        /**
         * @brief Get the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::DescriptionTexts& Draft description text
         */
        const Data::DescriptionTexts& getDescriptionTexts() const;

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
        void setDescriptionTexts(const Data::DescriptionTexts& texts);

        /**
         * @brief Get the item's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return const std::vector<Data::Affordance>& The item's affordances
         */
        const std::vector<Data::Affordance>& getAffordances() const;

        /**
         * @brief Set the item's affordances
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
         * @brief Check whether the item is marked as a container
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return bool True if the item is a container
         */
        bool isContainer() const;

        /**
         * @brief Mark or unmark the item as a container
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param container True to mark the item as a container
         */
        void setContainer(bool container);

        /**
         * @brief Get the IDs of items held inside this item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::vector<ADS::Types::ObjectId>& Contained item IDs
         */
        const std::vector<ADS::Types::ObjectId>& getContainerItemIds() const;

        /**
         * @brief Set the IDs of items held inside this item
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param ids The new contained item ID list
         */
        void setContainerItemIds(const std::vector<ADS::Types::ObjectId>& ids);

        /**
         * @brief Get the IDs of items this item can be combined with
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::vector<ADS::Types::ObjectId>& Combinable item IDs
         */
        const std::vector<ADS::Types::ObjectId>& getCombinableWithIds() const;

        /**
         * @brief Set the IDs of items this item can be combined with
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param ids The new combinable item ID list
         */
        void setCombinableWithIds(const std::vector<ADS::Types::ObjectId>& ids);

        /**
         * @brief Get the item's synonyms
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::vector<std::string>& Synonym words
         */
        const std::vector<std::string>& getSynonyms() const;

        /**
         * @brief Set the item's synonyms
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param synonyms The new synonym list
         */
        void setSynonyms(const std::vector<std::string>& synonyms);

        /**
         * @brief Get the item's abbreviations
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::vector<std::string>& Abbreviation words
         */
        const std::vector<std::string>& getAbbreviatures() const;

        /**
         * @brief Set the item's abbreviations
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param abbreviatures The new abbreviation list
         */
        void setAbbreviatures(const std::vector<std::string>& abbreviatures);

        /**
         * @brief Get the damage effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::Effect& The damage effect
         */
        const Data::Effect& getDamageEffect() const;

        /**
         * @brief Set the damage effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param effect The new damage effect
         */
        void setDamageEffect(const Data::Effect& effect);

        /**
         * @brief Get the heal effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Data::Effect& The heal effect
         */
        const Data::Effect& getHealEffect() const;

        /**
         * @brief Set the heal effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param effect The new heal effect
         */
        void setHealEffect(const Data::Effect& effect);

        /**
         * @brief Get the item's weight
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return uint8_t Weight, 0 (lightest) to 255 (heaviest)
         */
        uint8_t getWeight() const;

        /**
         * @brief Set the item's weight
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param weight The new weight, 0-255
         */
        void setWeight(uint8_t weight);

        /**
         * @brief Get the number of inventory slots this item occupies
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return uint8_t Slot count
         */
        uint8_t getSlots() const;

        /**
         * @brief Set the number of inventory slots this item occupies
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param slots The new slot count
         */
        void setSlots(uint8_t slots);

        /**
         * @brief Get the item's remaining service life
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return uint8_t Remaining service life, 0-255
         */
        uint8_t getServiceLife() const;

        /**
         * @brief Set the item's remaining service life
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param serviceLife The new remaining service life, 0-255
         */
        void setServiceLife(uint8_t serviceLife);

        /**
         * @brief Get the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Index into the getItemTypes() list
         */
        int getItemType() const;

        /**
         * @brief Set the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event (as EnumValue) if the value actually changed.
         *
         * @param type Index into the getItemTypes() list
         */
        void setItemType(int type);

        /**
         * @brief Get the path to the item image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& File path to the image
         */
        const std::string& getImagePath() const;

        /**
         * @brief Set the path to the item image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param path The new image file path
         */
        void setImagePath(const std::string& path);

        /**
         * @brief Get the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::optional<ADS::Types::SceneId>& Starting scene ID, or std::nullopt
         */
        const std::optional<ADS::Types::SceneId>& getInitialSceneId() const;

        /**
         * @brief Set the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Fires a property-changed event if the value actually changed.
         *
         * @param sceneId The new starting scene ID, or std::nullopt to clear it
         */
        void setInitialSceneId(const std::optional<ADS::Types::SceneId>& sceneId);

        /**
         * @brief Get the item's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
         */
        const std::optional<ADS::Types::StateId>& getState() const;

        /**
         * @brief Set the item's current state
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
         * @brief Get the item type name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Human-readable type name for the current item type index
         */
        std::string getItemTypeName() const;
    };
}

#endif //ADS_ITEM_ENTITY_H
