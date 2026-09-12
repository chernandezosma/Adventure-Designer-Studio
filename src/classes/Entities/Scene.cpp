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

/**
 * @file Scene.cpp
 * @brief Implementation of the Scene entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "Scene.h"
#include "Core/Project.h"
#include "image_formats.h"

namespace {
    /// One entry per named exit direction: the Inspector property id, its
    /// translation key, and a pointer-to-member into SceneData::Exits so
    /// getPropertyDescriptors()/getPropertyValue()/setPropertyValue() can
    /// loop instead of repeating ten near-identical blocks.
    struct ExitDirectionInfo {
        const char* propertyId;
        const char* translationKey;
        std::optional<ADS::Types::SceneId> ADS::Data::SceneData::Exits::*member;
    };

    const std::vector<ExitDirectionInfo>& exitDirections() {
        using Exits = ADS::Data::SceneData::Exits;
        static const std::vector<ExitDirectionInfo> directions = {
            {"exitNorth",     "SCENE.EXIT_NORTH",     &Exits::north},
            {"exitSouth",     "SCENE.EXIT_SOUTH",     &Exits::south},
            {"exitEast",      "SCENE.EXIT_EAST",      &Exits::east},
            {"exitWest",      "SCENE.EXIT_WEST",      &Exits::west},
            {"exitNortheast", "SCENE.EXIT_NORTHEAST", &Exits::northeast},
            {"exitNorthwest", "SCENE.EXIT_NORTHWEST", &Exits::northwest},
            {"exitSoutheast", "SCENE.EXIT_SOUTHEAST", &Exits::southeast},
            {"exitSouthwest", "SCENE.EXIT_SOUTHWEST", &Exits::southwest},
            {"exitUp",        "SCENE.EXIT_UP",        &Exits::up},
            {"exitDown",      "SCENE.EXIT_DOWN",      &Exits::down},
        };
        return directions;
    }

    /// Known affordance names from scene.md's Affordances definitions
    /// examples (Dark, Lightable, Visitable), offered as a quick-add
    /// preset list — see Scene::buildKnownAffordanceNames().
    const std::vector<const char*>& knownAffordanceKeys() {
        static const std::vector<const char*> keys = {
            "SCENE.AFF_DARK", "SCENE.AFF_LIGHTABLE", "SCENE.AFF_VISITABLE",
        };
        return keys;
    }

    /// One entry per Scene-applicable global trigger key
    /// (common-structures.md's Global triggers table — only on_enter,
    /// on_exit, and on_examine name Scene as applicable; on_turn is
    /// Game-only and on_item_taken/dropped/used are Item-only, each with
    /// their own trigger map now — see Entities::Item's own triggerKeys()).
    /// Options are always empty for now — no Event DataObject/catalog
    /// exists yet — forward-wired so this lights up automatically once one
    /// does, without any editor rework.
    struct TriggerKeyInfo {
        const char* propertyId;
        const char* translationKey;
        uint8_t triggerId;
    };

    const std::vector<TriggerKeyInfo>& triggerKeys() {
        static const std::vector<TriggerKeyInfo> keys = {
            {"triggerOnEnter",   "SCENE.TRIGGER_ON_ENTER",   0x01},
            {"triggerOnExit",    "SCENE.TRIGGER_ON_EXIT",    0x02},
            {"triggerOnExamine", "SCENE.TRIGGER_ON_EXAMINE", 0x03},
        };
        return keys;
    }

    /// Index of the scene with the given id within project->getScenes(),
    /// or -1 if absent/unresolvable. Must iterate in the same order as
    /// Scene::buildSceneOptionLabels() so indices line up.
    int findSceneOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::SceneId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& scenes = project->getScenes();
        for (size_t i = 0; i < scenes.size(); ++i) {
            if (scenes[i]->getSceneId() == *id) return static_cast<int>(i);
        }
        return -1;
    }

    /// Index of the item with the given id within project->getItems(),
    /// or -1 if absent/unresolvable. Must iterate in the same order as
    /// Scene::buildItemOptionLabels() so indices line up.
    int findItemOptionIndex(ADS::Core::Project* project, ADS::Types::ObjectId id) {
        if (!project) return -1;
        const auto& items = project->getItems();
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i]->getItemId() == id) return static_cast<int>(i);
        }
        return -1;
    }

    /// Index of the state with the given id within project->getStates(),
    /// or -1 if absent/unresolvable. Must iterate in the same order as
    /// Scene::buildStateOptionLabels() so indices line up.
    ///
    /// Unlike a State's own "next" (or a StateChain's own "head"), a
    /// Scene's "state" is just a reference — many scenes may legitimately
    /// start in the same state — so this lists every state, not just the
    /// "free" ones (see Project::getFreeStates(), which only matters for
    /// keeping a state chain's own internal links well-formed).
    int findStateOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::StateId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& states = project->getStates();
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->getStateId() == *id) return static_cast<int>(i);
        }
        return -1;
    }
}

namespace ADS::Entities {
    /**
     * @brief Construct a new Scene backed by the given SceneData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param data Non-owning pointer to the SceneData struct. Must not be
     *             null and must outlive this entity.
     */
    Scene::Scene(Data::SceneData *data) :
        BaseEntity(data), m_data(data)
    {
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Translated "Scene" type name
     */
    std::string Scene::getTypeName() const
    {
        return translate("SCENE.TYPE_NAME");
    }

    /**
     * @brief Get the list of property descriptors for this scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Returns metadata for all editable scene properties, organised by
     * category, for use by the inspector panel. descriptions/exits/items/
     * triggers are intentionally NOT exposed here — deferred to the
     * "Complex list UI" backlog item in CLAUDE.md.
     *
     * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
     */
    std::vector<Inspector::PropertyDescriptor> Scene::getPropertyDescriptors() const
    {
        using namespace Inspector;

        std::vector<PropertyDescriptor> descriptors = {
                // Info category (read-only) — rendered pinned above every
                // other category by InspectorPanel, regardless of position here.
                PropertyDescriptor("id", translate("SCENE.PROP_ID"), PropertyType::String)
                .setCategory(translate("CATEGORY.INFO"))
                .setDescription(translate("SCENE.DESC_ID"))
                .setReadOnly(),

                // General category
                PropertyDescriptor("name", translate("SCENE.PROP_NAME"), PropertyType::String)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("SCENE.DESC_NAME"))
                .setConstraints(PropertyConstraints::string(128)),

                PropertyDescriptor("isStartScene", translate("SCENE.PROP_START_SCENE"), PropertyType::Bool)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("SCENE.DESC_START_SCENE")),

                PropertyDescriptor("state", translate("SCENE.PROP_STATE"), PropertyType::Select)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("SCENE.DESC_STATE"))
                .setOptionsProvider([this] { return buildStateOptionLabels(); })
                .setEmptyOptionsText("INSPECTOR.NO_STATES")
                .setAllowCreateNew(),

                // Affordances category — editable list of named affordances,
                // each with its own trigger-name list. Unrelated to the
                // fixed EventId-keyed "triggers" category below.
                PropertyDescriptor("affordances", translate("SCENE.PROP_AFFORDANCES"), PropertyType::AffordanceList)
                .setCategory(translate("CATEGORY.AFFORDANCES"))
                .setDescription(translate("SCENE.DESC_AFFORDANCES"))
                .setOptionsProvider([] { return buildKnownAffordanceNames(); })
                .setPresetKeys([] { return buildKnownAffordancePresetKeys(); }),

                // Appearance category
                PropertyDescriptor("image", translate("SCENE.PROP_IMAGE"), PropertyType::String)
                .setCategory(translate("CATEGORY.APPEARANCE"))
                .setDescription(translate("SCENE.DESC_IMAGE"))
                .setConstraints(PropertyConstraints::filePath(
                    ADS::Constants::ImageFormats::supportedExtensions())),

                // Descriptions category — author-typed draft text; separate
                // from the (not-yet-compiled) LexEngine ids on Descriptions.
                PropertyDescriptor("descriptionsNormal", translate("SCENE.PROP_DESC_NORMAL"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("SCENE.DESC_DESC_NORMAL"))
                .setConstraints(PropertyConstraints::translatableText(512)),

                PropertyDescriptor("descriptionsLongText", translate("SCENE.PROP_DESC_LONG"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("SCENE.DESC_DESC_LONG"))
                .setConstraints(PropertyConstraints::translatableText(512)),

                PropertyDescriptor("descriptionsOdor", translate("SCENE.PROP_DESC_ODOR"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("SCENE.DESC_DESC_ODOR"))
                .setConstraints(PropertyConstraints::translatableText(512)),

                PropertyDescriptor("descriptionsSound", translate("SCENE.PROP_DESC_SOUND"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("SCENE.DESC_DESC_SOUND"))
                .setConstraints(PropertyConstraints::translatableText(512)),

                // Items category
                PropertyDescriptor("presentItemIds", translate("SCENE.PROP_ITEMS"), PropertyType::Select)
                .setCategory(translate("CATEGORY.ITEMS"))
                .setDescription(translate("SCENE.DESC_ITEMS"))
                .setMultiSelect(true)
                .setOptionsProvider([this] { return buildItemOptionLabels(); }),
        };

        // Exits category — one nullable single-select property per direction
        for (const auto& dir : exitDirections()) {
            descriptors.push_back(
                PropertyDescriptor(dir.propertyId, translate(dir.translationKey), PropertyType::Select)
                .setCategory(translate("CATEGORY.EXITS"))
                .setDescription(translate("SCENE.DESC_EXIT"))
                .setOptionsProvider([this] { return buildSceneOptionLabels(); })
            );
        }

        // Triggers category — one multi-select property per global trigger
        // key; always shows "No options available" until an Event
        // DataObject/catalog exists (see triggerKeys() docs above).
        for (const auto& key : triggerKeys()) {
            descriptors.push_back(
                PropertyDescriptor(key.propertyId, translate(key.translationKey), PropertyType::Select)
                .setCategory(translate("CATEGORY.TRIGGERS"))
                .setDescription(translate("SCENE.DESC_TRIGGER"))
                .setMultiSelect(true)
                .setOptionsProvider([] { return std::vector<std::string>{}; })
            );
        }

        return descriptors;
    }

    /**
     * @brief Get the current value of a property by ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param propertyId The unique property identifier string
     * @return Inspector::PropertyValue Current value, or std::monostate if unknown
     */
    Inspector::PropertyValue Scene::getPropertyValue(const std::string &propertyId) const
    {
        if (propertyId == "name")
            return m_data->getName();
        if (propertyId == "isStartScene")
            return m_data->isStartScene();
        if (propertyId == "state") {
            std::vector<std::string> options = buildStateOptionLabels();
            std::vector<int> selected;
            int idx = findStateOptionIndex(getProject(), m_data->getState());
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }
        if (propertyId == "image")
            return m_data->getImage();
        if (propertyId == "id")
            return std::to_string(m_data->getId().value);
        if (propertyId == "descriptionsNormal")
            return m_data->getDescriptionTexts().normal;
        if (propertyId == "descriptionsLongText")
            return m_data->getDescriptionTexts().longText;
        if (propertyId == "descriptionsOdor")
            return m_data->getDescriptionTexts().odor;
        if (propertyId == "descriptionsSound")
            return m_data->getDescriptionTexts().sound;
        if (propertyId == "affordances") {
            Inspector::AffordanceListValue value;
            for (const auto& a : m_data->getAffordances()) {
                const std::string displayName = a.presetKey.empty() ? a.name : translate(a.presetKey);
                value.emplace_back(displayName, a.triggers, a.presetKey);
            }
            return value;
        }
        if (propertyId == "presentItemIds") {
            std::vector<std::string> options = buildItemOptionLabels();
            std::vector<int> selected;
            for (const auto& itemId : m_data->getPresentItemIds()) {
                int idx = findItemOptionIndex(getProject(), itemId);
                if (idx >= 0) selected.push_back(idx);
            }
            return Inspector::SelectValue(selected, options);
        }

        for (const auto& dir : exitDirections()) {
            if (propertyId != dir.propertyId) continue;
            std::vector<std::string> options = buildSceneOptionLabels();
            std::vector<int> selected;
            int idx = findSceneOptionIndex(getProject(), m_data->getExits().*dir.member);
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }

        for (const auto& key : triggerKeys()) {
            if (propertyId == key.propertyId) {
                // No Event catalog exists yet — nothing meaningful to
                // resolve indices against, see triggerKeys() docs above.
                return Inspector::SelectValue({}, {});
            }
        }

        return std::monostate{};
    }

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
    bool Scene::setPropertyValue(const std::string &propertyId, const Inspector::PropertyValue &value)
    {
        if (propertyId == "name") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setName(*str);
                return true;
            }
        } else if (propertyId == "isStartScene") {
            if (auto *b = std::get_if<bool>(&value)) {
                setStartScene(*b);
                return true;
            }
        } else if (propertyId == "state") {
            if (auto *sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::optional<ADS::Types::StateId> newTarget;
                if (!sel->selectedIndices.empty() && getProject()) {
                    const auto& states = getProject()->getStates();
                    int idx = sel->selectedIndices.front();
                    if (idx >= 0 && idx < static_cast<int>(states.size())) {
                        newTarget = states[idx]->getStateId();
                    }
                }
                setState(newTarget);
                return true;
            }
        } else if (propertyId == "image") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setImage(*str);
                return true;
            }
        } else if (propertyId == "affordances") {
            if (auto *list = std::get_if<Inspector::AffordanceListValue>(&value)) {
                std::vector<Data::Affordance> affordances;
                affordances.reserve(list->size());
                for (const auto& e : *list) {
                    affordances.push_back({e.name, e.triggers, e.presetKey});
                }
                setAffordances(affordances);
                return true;
            }
        } else if (propertyId == "descriptionsNormal") {
            if (auto *localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.normal = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        } else if (propertyId == "descriptionsLongText") {
            if (auto *localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.longText = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        } else if (propertyId == "descriptionsOdor") {
            if (auto *localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.odor = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        } else if (propertyId == "descriptionsSound") {
            if (auto *localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.sound = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        } else if (propertyId == "presentItemIds") {
            if (auto *sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::vector<ADS::Types::ObjectId> ids;
                if (getProject()) {
                    const auto& items = getProject()->getItems();
                    for (int idx : sel->selectedIndices) {
                        if (idx >= 0 && idx < static_cast<int>(items.size())) {
                            ids.push_back(items[idx]->getItemId());
                        }
                    }
                }
                setPresentItemIds(ids);
                return true;
            }
        } else {
            for (const auto& dir : exitDirections()) {
                if (propertyId != dir.propertyId) continue;
                if (auto *sel = std::get_if<Inspector::SelectValue>(&value)) {
                    std::optional<ADS::Types::SceneId> newTarget;
                    if (!sel->selectedIndices.empty() && getProject()) {
                        const auto& scenes = getProject()->getScenes();
                        int idx = sel->selectedIndices.front();
                        if (idx >= 0 && idx < static_cast<int>(scenes.size())) {
                            newTarget = scenes[idx]->getSceneId();
                        }
                    }
                    Data::SceneData::Exits exits = m_data->getExits();
                    exits.*dir.member = newTarget;
                    setExits(exits);
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * @brief Get the typed scene identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return ADS::Types::SceneId The scene's typed identifier
     */
    ADS::Types::SceneId Scene::getSceneId() const
    {
        return m_data->getId();
    }

    /**
     * @brief Build the "(id) name" option-label list for every scene in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::vector<std::string> One label per scene, in project order
     */
    std::vector<std::string> Scene::buildSceneOptionLabels() const
    {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& scene : getProject()->getScenes()) {
            labels.push_back(scene->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Build the "(id) name" option-label list for every item in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::vector<std::string> One label per item, in project order
     */
    std::vector<std::string> Scene::buildItemOptionLabels() const
    {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& item : getProject()->getItems()) {
            labels.push_back(item->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Build the option-label list for every state in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::vector<std::string> One label per state, in project order
     */
    std::vector<std::string> Scene::buildStateOptionLabels() const
    {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& state : getProject()->getStates()) {
            labels.push_back(state->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Build the preset list of known affordance names
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return std::vector<std::string> Known affordance names
     */
    std::vector<std::string> Scene::buildKnownAffordanceNames()
    {
        std::vector<std::string> labels;
        for (const char* key : knownAffordanceKeys()) {
            labels.push_back(translate(key));
        }
        return labels;
    }

    /**
     * @brief Build the preset list of known affordance translation keys
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return std::vector<std::string> Known affordance translation keys
     */
    std::vector<std::string> Scene::buildKnownAffordancePresetKeys()
    {
        std::vector<std::string> keys;
        for (const char* key : knownAffordanceKeys()) {
            keys.emplace_back(key);
        }
        return keys;
    }

    /**
     * @brief Get the sensory descriptions of the scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const Data::Descriptions& LexEngine text-id references for this scene
     */
    const Data::Descriptions& Scene::getDescriptions() const
    {
        return m_data->getDescriptions();
    }

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
    void Scene::setDescriptions(const Data::Descriptions &descriptions)
    {
        if (m_data->getDescriptions() != descriptions) {
            m_data->setDescriptions(descriptions);
            notifyPropertyChanged("descriptions", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the author-typed draft text for the scene's descriptions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const Data::DescriptionTexts& Draft description text
     */
    const Data::DescriptionTexts& Scene::getDescriptionTexts() const
    {
        return m_data->getDescriptionTexts();
    }

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
    void Scene::setDescriptionTexts(const Data::DescriptionTexts &texts)
    {
        if (m_data->getDescriptionTexts() != texts) {
            m_data->setDescriptionTexts(texts);
            notifyPropertyChanged("descriptionTexts", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the scene's directional exits
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const Data::SceneData::Exits& The ten named directional exits
     */
    const Data::SceneData::Exits& Scene::getExits() const
    {
        return m_data->getExits();
    }

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
    void Scene::setExits(const Data::SceneData::Exits &exits)
    {
        if (m_data->getExits() != exits) {
            m_data->setExits(exits);
            notifyPropertyChanged("exits", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the IDs of items present in this scene at load time
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::vector<ADS::Types::ObjectId>& Present item IDs
     */
    const std::vector<ADS::Types::ObjectId>& Scene::getPresentItemIds() const
    {
        return m_data->getPresentItemIds();
    }

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
    void Scene::setPresentItemIds(const std::vector<ADS::Types::ObjectId> &ids)
    {
        if (m_data->getPresentItemIds() != ids) {
            m_data->setPresentItemIds(ids);
            notifyPropertyChanged("presentItemIds", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the scene's trigger map
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Trigger map
     */
    const std::map<uint8_t, std::vector<ADS::Types::EventId>>& Scene::getTriggers() const
    {
        return m_data->getTriggers();
    }

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
    void Scene::setTriggers(const std::map<uint8_t, std::vector<ADS::Types::EventId>> &triggers)
    {
        if (m_data->getTriggers() != triggers) {
            m_data->setTriggers(triggers);
            notifyPropertyChanged("triggers", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Append an EventId handler for a global trigger
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param eventId Global trigger id (e.g. 0x01 = on_enter)
     * @param handler EventId to append to that trigger's handler list
     */
    void Scene::addTrigger(uint8_t eventId, ADS::Types::EventId handler)
    {
        m_data->addTrigger(eventId, handler);
        notifyPropertyChanged("triggers", std::monostate{}, std::monostate{});
    }

    /**
     * @brief Get the scene's affordances
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return const std::vector<Data::Affordance>& The scene's affordances
     */
    const std::vector<Data::Affordance>& Scene::getAffordances() const
    {
        return m_data->getAffordances();
    }

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
    void Scene::setAffordances(const std::vector<Data::Affordance>& affordances)
    {
        if (m_data->getAffordances() != affordances) {
            m_data->setAffordances(affordances);
            notifyPropertyChanged("affordances", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the filename or base64-encoded image for this scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::string& Image filename or base64 payload
     */
    const std::string& Scene::getImage() const
    {
        return m_data->getImage();
    }

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
    void Scene::setImage(const std::string &image)
    {
        setAndNotify("image",
                     [this] { return m_data->getImage(); },
                     [this](const std::string &v) { m_data->setImage(v); },
                     image);
    }

    /**
     * @brief Get the scene's current state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
     */
    const std::optional<ADS::Types::StateId>& Scene::getState() const
    {
        return m_data->getState();
    }

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
    void Scene::setState(const std::optional<ADS::Types::StateId> &state)
    {
        if (m_data->getState() != state) {
            auto toStr = [](const std::optional<ADS::Types::StateId>& id) {
                return id.has_value() ? std::to_string(id->value) : std::string();
            };
            std::string oldStr = toStr(m_data->getState());
            m_data->setState(state);
            notifyPropertyChanged("state", oldStr, toStr(state));
        }
    }

    /**
     * @brief Check whether this is the game's starting scene
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return bool True if this is the starting scene
     */
    bool Scene::isStartScene() const
    {
        return m_data->isStartScene();
    }

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
    void Scene::setStartScene(bool isStart)
    {
        setAndNotify("isStartScene",
                     [this] {
                         return m_data->isStartScene();
                     },
                     [this](bool v) {
                         m_data->setStartScene(v);
                     },
                     isStart);
    }
}
