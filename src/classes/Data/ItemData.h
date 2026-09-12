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

#ifndef ADS_DATA_ITEM_DATA_H
#define ADS_DATA_ITEM_DATA_H

/**
 * @file ItemData.h
 * @brief Pure data class for a game item — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Affordance.h"
#include "BaseData.h"
#include "Descriptions.h"
#include "Effect.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a game item.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Holds all persistent data for an item. Only standard C++ types are
     * used — no ImGui or UI framework types appear here. This class is owned
     * by Core::Project; Entities::Item reads/writes through a non-owning
     * pointer to this object. Field set follows docs/core/schemas/item.md.
     */
    class ItemData : public BaseData<ADS::Types::ObjectTag> {
    public:
        /**
         * @brief Get the item's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Descriptions& LexEngine text-id references for this item
         */
        [[nodiscard]] const Descriptions& getDescriptions() const { return m_descriptions; }

        /**
         * @brief Set the item's sensory descriptions (LexEngine text ids)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param descriptions The new descriptions
         */
        void setDescriptions(const Descriptions& descriptions) { m_descriptions = descriptions; }

        /**
         * @brief Get the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const DescriptionTexts& Draft description text, per language
         */
        [[nodiscard]] const DescriptionTexts& getDescriptionTexts() const { return m_descriptionTexts; }

        /**
         * @brief Set the author-typed draft text backing the descriptions group
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param texts The new draft description text
         */
        void setDescriptionTexts(const DescriptionTexts& texts) { m_descriptionTexts = texts; }

        /**
         * @brief Get the per-language name overrides.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * IDE-only, not in `.ads` — persisted to `.trn`. The default-language
         * name lives in BaseData::m_name; this map holds the other languages.
         *
         * @return const LocalizedText& langCode -> translated name
         */
        [[nodiscard]] const LocalizedText& getNameTexts() const { return m_nameTexts; }

        /**
         * @brief Replace the per-language name overrides.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param texts New langCode -> name map
         */
        void setNameTexts(const LocalizedText& texts) { m_nameTexts = texts; }

        /**
         * @brief Get the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return int Item type index (Generic, Key, Weapon, etc.)
         */
        [[nodiscard]] int getItemType() const { return m_itemType; }

        /**
         * @brief Set the item type index
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param itemType The new item type index
         */
        void setItemType(int itemType) { m_itemType = itemType; }

        /**
         * @brief Get the item's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Author-named affordances and the trigger names each one fires —
         * see Data::Affordance. Replaces the old fixed 32-bit bitmap.
         *
         * @return const std::vector<Affordance>& The item's affordances
         */
        [[nodiscard]] const std::vector<Affordance>& getAffordances() const { return m_affordances; }

        /**
         * @brief Set the item's affordances
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param affordances The new affordance list
         */
        void setAffordances(const std::vector<Affordance>& affordances) { m_affordances = affordances; }

        /**
         * @brief Check whether the item is marked as a container
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Independent of the affordance list — item.md's former "is a
         * container" bit, promoted to its own field.
         *
         * @return bool True if the item is a container
         */
        [[nodiscard]] bool isContainer() const { return m_isContainer; }

        /**
         * @brief Mark or unmark the item as a container
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param container True to mark the item as a container
         */
        void setContainer(bool container) { m_isContainer = container; }

        /**
         * @brief Get the item's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Author-set reference into the project's shared State catalog.
         * std::nullopt means no state applies. Same treatment as
         * SceneData/CharacterData: a plain reference, not restricted to
         * "free" states.
         *
         * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
         */
        [[nodiscard]] const std::optional<ADS::Types::StateId>& getState() const { return m_state; }

        /**
         * @brief Set the item's current state
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param state The new state id, or std::nullopt to clear it
         */
        void setState(const std::optional<ADS::Types::StateId>& state) { m_state = state; }

        /**
         * @brief Get the item's weight
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Per item.md: 0 lightest to 255 heaviest.
         *
         * @return uint8_t Weight, 0-255
         */
        [[nodiscard]] uint8_t getWeight() const { return m_weight; }

        /**
         * @brief Set the item's weight
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param weight The new weight, 0-255
         */
        void setWeight(uint8_t weight) { m_weight = weight; }

        /**
         * @brief Get the number of inventory slots this item occupies
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return uint8_t Slot count
         */
        [[nodiscard]] uint8_t getSlots() const { return m_slots; }

        /**
         * @brief Set the number of inventory slots this item occupies
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param slots The new slot count
         */
        void setSlots(uint8_t slots) { m_slots = slots; }

        /**
         * @brief Get the item's remaining service life
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Per item.md: when this reaches 0, the item is destroyed.
         *
         * @return uint8_t Remaining service life, 0-255
         */
        [[nodiscard]] uint8_t getServiceLife() const { return m_serviceLife; }

        /**
         * @brief Set the item's remaining service life
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param serviceLife The new remaining service life, 0-255
         */
        void setServiceLife(uint8_t serviceLife) { m_serviceLife = serviceLife; }

        /**
         * @brief Get the path to the item image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * item.md's `image` field — filename or encoded string.
         *
         * @return const std::string& File path to the image
         */
        [[nodiscard]] const std::string& getImagePath() const { return m_imagePath; }

        /**
         * @brief Set the path to the item image
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param path The new image file path
         */
        void setImagePath(const std::string& path) { m_imagePath = path; }

        /**
         * @brief Get the IDs of items held inside this item (container contents)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * item.md's `container` field. Only meaningful when the "is a
         * container" affordance bit is set.
         *
         * @return const std::vector<ADS::Types::ObjectId>& Contained item IDs
         */
        [[nodiscard]] const std::vector<ADS::Types::ObjectId>& getContainerItemIds() const { return m_containerItemIds; }

        /**
         * @brief Set the IDs of items held inside this item (container contents)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param ids The new contained item ID list
         */
        void setContainerItemIds(const std::vector<ADS::Types::ObjectId>& ids) { m_containerItemIds = ids; }

        /**
         * @brief Get the IDs of items this item can be combined with
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * item.md's `combinable_with` field.
         *
         * @return const std::vector<ADS::Types::ObjectId>& Combinable item IDs
         */
        [[nodiscard]] const std::vector<ADS::Types::ObjectId>& getCombinableWithIds() const { return m_combinableWithIds; }

        /**
         * @brief Set the IDs of items this item can be combined with
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param ids The new combinable item ID list
         */
        void setCombinableWithIds(const std::vector<ADS::Types::ObjectId>& ids) { m_combinableWithIds = ids; }

        /**
         * @brief Get the item's synonyms (alternative parser words)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * item.md's `language.synonyms` — authoring-time list of strings;
         * compiled to LexEngine ids later.
         *
         * @return const std::vector<std::string>& Synonym words
         */
        [[nodiscard]] const std::vector<std::string>& getSynonyms() const { return m_synonyms; }

        /**
         * @brief Set the item's synonyms (alternative parser words)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param synonyms The new synonym list
         */
        void setSynonyms(const std::vector<std::string>& synonyms) { m_synonyms = synonyms; }

        /**
         * @brief Get the item's abbreviations (short parser forms)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * item.md's `language.abbreviatures`.
         *
         * @return const std::vector<std::string>& Abbreviation words
         */
        [[nodiscard]] const std::vector<std::string>& getAbbreviatures() const { return m_abbreviatures; }

        /**
         * @brief Set the item's abbreviations (short parser forms)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param abbreviatures The new abbreviation list
         */
        void setAbbreviatures(const std::vector<std::string>& abbreviatures) { m_abbreviatures = abbreviatures; }

        /**
         * @brief Get the damage effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Effect& The damage effect
         */
        [[nodiscard]] const Effect& getDamageEffect() const { return m_damage; }

        /**
         * @brief Set the damage effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param effect The new damage effect
         */
        void setDamageEffect(const Effect& effect) { m_damage = effect; }

        /**
         * @brief Get the heal effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const Effect& The heal effect
         */
        [[nodiscard]] const Effect& getHealEffect() const { return m_heal; }

        /**
         * @brief Set the heal effect this item applies when used
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param effect The new heal effect
         */
        void setHealEffect(const Effect& effect) { m_heal = effect; }

        /**
         * @brief Get the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return const std::optional<ADS::Types::SceneId>& Starting scene ID,
         *         or std::nullopt if the item has no starting scene
         */
        [[nodiscard]] const std::optional<ADS::Types::SceneId>& getInitialSceneId() const { return m_initialSceneId; }

        /**
         * @brief Set the ID of the scene where this item starts
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param sceneId The new starting scene ID, or std::nullopt to clear it
         */
        void setInitialSceneId(const std::optional<ADS::Types::SceneId>& sceneId) { m_initialSceneId = sceneId; }

        /**
         * @brief Get the item's trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Maps an item-applicable global trigger id (see
         * common-structures.md's Global triggers table, e.g. 0x03 =
         * on_examine) to an ordered array of EventIds. Independent of
         * Scene's own trigger map — same shape, separate instance.
         *
         * @return const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Trigger map
         */
        [[nodiscard]] const std::map<uint8_t, std::vector<ADS::Types::EventId>>& getTriggers() const { return m_triggers; }

        /**
         * @brief Replace the item's entire trigger map
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param triggers The new trigger map
         */
        void setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>>& triggers) { m_triggers = triggers; }

        /**
         * @brief Append an EventId handler for a global trigger
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Appends to the existing array for @p eventId if one already
         * exists, otherwise creates it. Never overwrites.
         *
         * @param eventId Global trigger id (e.g. 0x03 = on_examine)
         * @param handler EventId to append to that trigger's handler list
         */
        void addTrigger(uint8_t eventId, ADS::Types::EventId handler) { m_triggers[eventId].push_back(handler); }

    private:
        Descriptions      m_descriptions;                      ///< LexEngine text-id references (item.md descriptions)
        DescriptionTexts  m_descriptionTexts;                  ///< IDE-only draft text backing m_descriptions
        LocalizedText     m_nameTexts;                         ///< IDE-only per-language name overrides (.trn only)
        int               m_itemType     = 0;                  ///< Item type index (Generic, Key, Weapon, etc.)
        std::vector<Affordance> m_affordances;                 ///< Author-named affordances and their triggers
        bool              m_isContainer  = false;               ///< Whether this item is a container
        std::optional<ADS::Types::StateId> m_state;             ///< Author-set reference into the project's State catalog
        uint8_t           m_weight       = 0;                  ///< Weight, 0 (lightest) to 255 (heaviest)
        uint8_t           m_slots        = 1;                  ///< Inventory slots occupied
        uint8_t           m_serviceLife  = 0;                  ///< Remaining service life; 0 = destroyed
        std::string       m_imagePath;                         ///< item.md image
        std::vector<ADS::Types::ObjectId> m_containerItemIds;   ///< item.md container contents
        std::vector<ADS::Types::ObjectId> m_combinableWithIds;  ///< item.md combinable_with
        std::vector<std::string> m_synonyms;                   ///< item.md language.synonyms
        std::vector<std::string> m_abbreviatures;              ///< item.md language.abbreviatures
        Effect            m_damage;                             ///< item.md effects.damage
        Effect            m_heal;                               ///< item.md effects.heal
        std::optional<ADS::Types::SceneId> m_initialSceneId;    ///< ID of the scene where this item starts
        std::map<uint8_t, std::vector<ADS::Types::EventId>> m_triggers; ///< Global trigger id -> EventId handlers
    };

} // namespace ADS::Data

#endif // ADS_DATA_ITEM_DATA_H
