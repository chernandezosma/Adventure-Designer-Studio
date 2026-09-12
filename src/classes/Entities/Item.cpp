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
 * @file Item.cpp
 * @brief Implementation of the Item entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include "Item.h"
#include "Core/Project.h"
#include "image_formats.h"

#include <sstream>

namespace {
    using ADS::Types::StateId;
    using ADS::Types::ObjectId;

    /// Index of ITEM.TYPE_CONTAINER within Item::getItemTypes() (the list
    /// order is a fixed contract — see getItemType()). Type == Container is
    /// kept in sync with ItemData::isContainer().
    constexpr int kContainerTypeIndex = 7;

    /// Index of the state with the given id within project->getStates(),
    /// or -1 if absent/unresolvable. Mirrors the same helper in Scene/Character.
    int findStateOptionIndex(ADS::Core::Project* project, const std::optional<StateId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& states = project->getStates();
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->getStateId() == *id) return static_cast<int>(i);
        }
        return -1;
    }

    /// Index of the scene with the given id within project->getScenes(),
    /// or -1 if absent/unresolvable. Must iterate in the same order as
    /// Item::buildSceneOptionLabels(). Mirrors Scene's findSceneOptionIndex().
    int findSceneOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::SceneId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& scenes = project->getScenes();
        for (size_t i = 0; i < scenes.size(); ++i) {
            if (scenes[i]->getSceneId() == *id) return static_cast<int>(i);
        }
        return -1;
    }


    /// Known affordance names from item.md's former fixed bitmap, offered
    /// as a quick-add preset list — see Item::buildKnownAffordanceNames().
    /// "is a container" is not included: it is the dedicated "isContainer"
    /// boolean, not an affordance entry.
    const std::vector<const char*>& knownAffordanceKeys() {
        static const std::vector<const char*> keys = {
            "ITEM.AFF_TAKEABLE", "ITEM.AFF_DROPPABLE", "ITEM.AFF_WEARABLE",
            "ITEM.AFF_CONSUMABLE", "ITEM.AFF_OPENABLE", "ITEM.AFF_LOCKABLE",
            "ITEM.AFF_BREAKABLE", "ITEM.AFF_MOVABLE", "ITEM.AFF_READABLE",
            "ITEM.AFF_LIGHTABLE", "ITEM.AFF_THROWABLE", "ITEM.AFF_GIVEABLE",
            "ITEM.AFF_COMBINABLE", "ITEM.AFF_FILLABLE", "ITEM.AFF_BURNABLE",
            "ITEM.AFF_CUTTABLE", "ITEM.AFF_WRITABLE", "ITEM.AFF_LISTENABLE",
            "ITEM.AFF_SMELLABLE", "ITEM.AFF_EXAMINABLE", "ITEM.AFF_SHOWABLE",
            "ITEM.AFF_HIDEABLE", "ITEM.AFF_SINGLE_USE", "ITEM.AFF_MAGIC",
        };
        return keys;
    }

    /// One entry per item-applicable global trigger key (common-structures.md's
    /// Global triggers table). Options are always empty for now — no Event
    /// DataObject/catalog exists yet — forward-wired so this lights up
    /// automatically once one does, without any editor rework. Same ids
    /// Scene uses for the shared concepts (on_examine, on_item_*): they are
    /// the same global event, just fired/listened-to from the item's own side.
    struct TriggerKeyInfo {
        const char* propertyId;
        const char* translationKey;
        uint8_t triggerId;
    };

    const std::vector<TriggerKeyInfo>& triggerKeys() {
        static const std::vector<TriggerKeyInfo> keys = {
            {"triggerOnExamine",     "ITEM.TRIGGER_ON_EXAMINE",      0x03},
            {"triggerOnItemTaken",   "ITEM.TRIGGER_ON_ITEM_TAKEN",   0x05},
            {"triggerOnItemDropped", "ITEM.TRIGGER_ON_ITEM_DROPPED", 0x06},
            {"triggerOnItemUsed",    "ITEM.TRIGGER_ON_ITEM_USED",    0x07},
        };
        return keys;
    }

    /// Split newline-separated text into non-empty trimmed lines.
    std::vector<std::string> splitLines(const std::string& text) {
        std::vector<std::string> out;
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line)) {
            size_t begin = line.find_first_not_of(" \t\r");
            if (begin == std::string::npos) continue;
            size_t end = line.find_last_not_of(" \t\r");
            out.push_back(line.substr(begin, end - begin + 1));
        }
        return out;
    }

    /// Join a word list back into newline-separated text.
    std::string joinLines(const std::vector<std::string>& words) {
        std::string out;
        for (size_t i = 0; i < words.size(); ++i) {
            if (i > 0) out += '\n';
            out += words[i];
        }
        return out;
    }
}

namespace ADS::Entities {
    /**
     * @brief Get the list of available item type names
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::vector<std::string> Available item type names
     */
    std::vector<std::string> Item::getItemTypes() {
        return {
            translate("ITEM.TYPE_GENERIC"),
            translate("ITEM.TYPE_KEY"),
            translate("ITEM.TYPE_WEAPON"),
            translate("ITEM.TYPE_ARMOR"),
            translate("ITEM.TYPE_CONSUMABLE"),
            translate("ITEM.TYPE_QUEST_ITEM"),
            translate("ITEM.TYPE_DOCUMENT"),
            translate("ITEM.TYPE_CONTAINER")
        };
    }

    /**
     * @brief Construct a new Item backed by the given ItemData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param data Non-owning pointer to the ItemData struct. Must not be
     *             null and must outlive this entity.
     */
    Item::Item(Data::ItemData* data)
        : BaseEntity(data), m_data(data) {
    }

    /**
     * @brief Get the typed item identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return ADS::Types::ObjectId The item's typed identifier
     */
    ADS::Types::ObjectId Item::getItemId() const {
        return m_data->getId();
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Translated "Item" type name
     */
    std::string Item::getTypeName() const {
        return translate("ITEM.TYPE_NAME");
    }

    /**
     * @brief Get the list of property descriptors for this item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
     */
    std::vector<Inspector::PropertyDescriptor> Item::getPropertyDescriptors() const {
        using namespace Inspector;

        // An item can never occupy more slots than the whole inventory holds
        // (Data::GameData::getInventoryCapacity()); fall back to the uint8_t
        // ceiling when the item is not attached to a project.
        const float maxSlots = getProject()
            ? static_cast<float>(getProject()->getGameData().getInventoryCapacity())
            : 255.0f;

        auto effectRow = [](const char* id, const char* key, const char* descKey,
                            const char* subcatKey) {
            return PropertyDescriptor(id, translate(key), PropertyType::Int)
                .setCategory(translate("CATEGORY.EFFECTS"))
                .setSubcategory(translate(subcatKey))
                .setDescription(translate(descKey))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1));
        };

        std::vector<PropertyDescriptor> descriptors = {
            // Info category (read-only) — pinned above every other category.
            PropertyDescriptor("id", translate("ITEM.PROP_ID"), PropertyType::String)
                .setCategory(translate("CATEGORY.INFO"))
                .setDescription(translate("ITEM.DESC_ID"))
                .setReadOnly(),

            // General category
            PropertyDescriptor("name", translate("ITEM.PROP_NAME"), PropertyType::String)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("ITEM.DESC_NAME"))
                .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("itemType", translate("ITEM.PROP_TYPE"), PropertyType::Enum)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("ITEM.DESC_TYPE"))
                .setConstraints(PropertyConstraints::enumeration(getItemTypes())),

            PropertyDescriptor("initialSceneId", translate("ITEM.PROP_INITIAL_SCENE"), PropertyType::Select)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("ITEM.DESC_INITIAL_SCENE"))
                .setOptionsProvider([this] { return buildSceneOptionLabels(); }),

            PropertyDescriptor("state", translate("ITEM.PROP_STATE"), PropertyType::Select)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("ITEM.DESC_STATE"))
                .setOptionsProvider([this] { return buildStateOptionLabels(); })
                .setEmptyOptionsText("INSPECTOR.NO_STATES")
                .setAllowCreateNew(),

            // Affordances category — editable list of named affordances,
            // each with its own trigger-name list.
            PropertyDescriptor("affordances", translate("ITEM.PROP_AFFORDANCES"), PropertyType::AffordanceList)
                .setCategory(translate("CATEGORY.AFFORDANCES"))
                .setDescription(translate("ITEM.DESC_AFFORDANCES"))
                .setOptionsProvider([] { return buildKnownAffordanceNames(); })
                .setPresetKeys([] { return buildKnownAffordancePresetKeys(); }),

            // Relations category
            PropertyDescriptor("isContainer", translate("ITEM.PROP_IS_CONTAINER"), PropertyType::Bool)
                .setCategory(translate("CATEGORY.RELATIONS"))
                .setDescription(translate("ITEM.DESC_IS_CONTAINER")),

            // Contained-items list — only shown once the item is marked as a
            // container (item.md affordance bit 23). At runtime the game fills
            // this; here it lets the author pre-place items inside.
            PropertyDescriptor("containerItems", translate("ITEM.PROP_CONTAINER_ITEMS"), PropertyType::Select)
                .setCategory(translate("CATEGORY.RELATIONS"))
                .setDescription(translate("ITEM.DESC_CONTAINER_ITEMS"))
                .setMultiSelect(true)
                .setOptionsProvider([this] { return buildItemOptionLabels(); })
                .setVisibilityCondition([this](const Inspector::IInspectable*) {
                    return m_data->isContainer();
                }),

            PropertyDescriptor("combinableWith", translate("ITEM.PROP_COMBINABLE_WITH"), PropertyType::Select)
                .setCategory(translate("CATEGORY.RELATIONS"))
                .setDescription(translate("ITEM.DESC_COMBINABLE_WITH"))
                .setMultiSelect(true)
                .setOptionsProvider([this] { return buildItemOptionLabels(); }),

            // Stats category
            PropertyDescriptor("weight", translate("ITEM.PROP_WEIGHT"), PropertyType::Int)
                .setCategory(translate("CATEGORY.STATS"))
                .setDescription(translate("ITEM.DESC_WEIGHT"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            PropertyDescriptor("slots", translate("ITEM.PROP_SLOTS"), PropertyType::Int)
                .setCategory(translate("CATEGORY.STATS"))
                .setDescription(translate("ITEM.DESC_SLOTS"))
                .setConstraints(PropertyConstraints::numeric(0, maxSlots, 1)),

            PropertyDescriptor("serviceLife", translate("ITEM.PROP_SERVICE_LIFE"), PropertyType::Int)
                .setCategory(translate("CATEGORY.STATS"))
                .setDescription(translate("ITEM.DESC_SERVICE_LIFE"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            // Language category
            PropertyDescriptor("synonyms", translate("ITEM.PROP_SYNONYMS"), PropertyType::String)
                .setCategory(translate("CATEGORY.LANGUAGE"))
                .setDescription(translate("ITEM.DESC_SYNONYMS"))
                .setConstraints(PropertyConstraints::string(512, true)),

            PropertyDescriptor("abbreviatures", translate("ITEM.PROP_ABBREVIATURES"), PropertyType::String)
                .setCategory(translate("CATEGORY.LANGUAGE"))
                .setDescription(translate("ITEM.DESC_ABBREVIATURES"))
                .setConstraints(PropertyConstraints::string(512, true)),

            // Appearance category
            PropertyDescriptor("imagePath", translate("ITEM.PROP_IMAGE"), PropertyType::String)
                .setCategory(translate("CATEGORY.APPEARANCE"))
                .setDescription(translate("ITEM.DESC_IMAGE"))
                .setConstraints(PropertyConstraints::filePath(
                    ADS::Constants::ImageFormats::supportedExtensions())),

            // Descriptions category
            PropertyDescriptor("descriptionsNormal", translate("ITEM.PROP_DESC_NORMAL"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("ITEM.DESC_DESC_NORMAL"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsLongText", translate("ITEM.PROP_DESC_LONG"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("ITEM.DESC_DESC_LONG"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsOdor", translate("ITEM.PROP_DESC_ODOR"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("ITEM.DESC_DESC_ODOR"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsSound", translate("ITEM.PROP_DESC_SOUND"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("ITEM.DESC_DESC_SOUND"))
                .setConstraints(PropertyConstraints::translatableText(512)),
        };

        // Effects category — a "Damage" then a "Heal" subgroup, per
        // common-structures.md#effects. The subcategory drives a nested
        // collapsible node in the inspector (InspectorPanel::renderCategory).
        // The subgroup node ("Damage" / "Heal") already carries the context, so
        // the leaf labels are the bare field names, shared by both groups.
        const char* kDmg  = "ITEM.EFFECT_GROUP_DAMAGE";
        const char* kHeal = "ITEM.EFFECT_GROUP_HEAL";
        descriptors.push_back(effectRow("dmgAppliedTo",   "ITEM.PROP_EFFECT_APPLIED_TO", "ITEM.DESC_EFFECT_APPLIED_TO",  kDmg));
        descriptors.push_back(effectRow("dmgBase",        "ITEM.PROP_EFFECT_BASE",       "ITEM.DESC_EFFECT_BASE",        kDmg));
        descriptors.push_back(effectRow("dmgCritChance",  "ITEM.PROP_EFFECT_CRIT_CHANCE","ITEM.DESC_EFFECT_CRIT_CHANCE", kDmg));
        descriptors.push_back(effectRow("dmgCritMult",    "ITEM.PROP_EFFECT_CRIT_MULT",  "ITEM.DESC_EFFECT_CRIT_MULT",   kDmg));
        descriptors.push_back(effectRow("dmgRate",        "ITEM.PROP_EFFECT_RATE",       "ITEM.DESC_EFFECT_RATE",        kDmg));
        descriptors.push_back(effectRow("dmgUnit",        "ITEM.PROP_EFFECT_UNIT",       "ITEM.DESC_EFFECT_UNIT",        kDmg));
        descriptors.push_back(effectRow("healAppliedTo",  "ITEM.PROP_EFFECT_APPLIED_TO", "ITEM.DESC_EFFECT_APPLIED_TO",  kHeal));
        descriptors.push_back(effectRow("healBase",       "ITEM.PROP_EFFECT_BASE",       "ITEM.DESC_EFFECT_BASE",        kHeal));
        descriptors.push_back(effectRow("healCritChance", "ITEM.PROP_EFFECT_CRIT_CHANCE","ITEM.DESC_EFFECT_CRIT_CHANCE", kHeal));
        descriptors.push_back(effectRow("healCritMult",   "ITEM.PROP_EFFECT_CRIT_MULT",  "ITEM.DESC_EFFECT_CRIT_MULT",   kHeal));
        descriptors.push_back(effectRow("healRate",       "ITEM.PROP_EFFECT_RATE",       "ITEM.DESC_EFFECT_RATE",        kHeal));
        descriptors.push_back(effectRow("healUnit",       "ITEM.PROP_EFFECT_UNIT",       "ITEM.DESC_EFFECT_UNIT",        kHeal));

        // Triggers category — one multi-select property per item-applicable
        // global trigger key; always shows "No options available" until an
        // Event DataObject/catalog exists (see triggerKeys() docs above).
        for (const auto& key : triggerKeys()) {
            descriptors.push_back(
                PropertyDescriptor(key.propertyId, translate(key.translationKey), PropertyType::Select)
                .setCategory(translate("CATEGORY.TRIGGERS"))
                .setDescription(translate("ITEM.DESC_TRIGGER"))
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
     * @version Aug 2026
     *
     * @param propertyId The unique property identifier string
     * @return Inspector::PropertyValue Current value, or std::monostate if unknown
     */
    Inspector::PropertyValue Item::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name")        return m_data->getName();
        if (propertyId == "isContainer") return m_data->isContainer();
        if (propertyId == "weight")      return static_cast<int>(m_data->getWeight());
        if (propertyId == "slots")       return static_cast<int>(m_data->getSlots());
        if (propertyId == "serviceLife") return static_cast<int>(m_data->getServiceLife());
        if (propertyId == "id")          return std::to_string(m_data->getId().value);
        if (propertyId == "imagePath")   return m_data->getImagePath();
        if (propertyId == "synonyms")      return joinLines(m_data->getSynonyms());
        if (propertyId == "abbreviatures") return joinLines(m_data->getAbbreviatures());
        if (propertyId == "descriptionsNormal")   return m_data->getDescriptionTexts().normal;
        if (propertyId == "descriptionsLongText") return m_data->getDescriptionTexts().longText;
        if (propertyId == "descriptionsOdor")     return m_data->getDescriptionTexts().odor;
        if (propertyId == "descriptionsSound")    return m_data->getDescriptionTexts().sound;
        if (propertyId == "initialSceneId") {
            std::vector<std::string> options = buildSceneOptionLabels();
            std::vector<int> selected;
            int idx = findSceneOptionIndex(getProject(), m_data->getInitialSceneId());
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }
        if (propertyId == "itemType") {
            return Inspector::EnumValue(m_data->getItemType(), getItemTypes());
        }
        if (propertyId == "state") {
            std::vector<std::string> options = buildStateOptionLabels();
            std::vector<int> selected;
            int idx = findStateOptionIndex(getProject(), m_data->getState());
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }
        if (propertyId == "affordances") {
            Inspector::AffordanceListValue value;
            for (const auto& a : m_data->getAffordances()) {
                const std::string displayName = a.presetKey.empty() ? a.name : translate(a.presetKey);
                value.emplace_back(displayName, a.triggers, a.presetKey);
            }
            return value;
        }
        if (propertyId == "containerItems" || propertyId == "combinableWith") {
            const auto& ids = propertyId == "containerItems"
                ? m_data->getContainerItemIds() : m_data->getCombinableWithIds();
            const std::vector<Item*> opts = relationItemOptions();
            std::vector<std::string> options;
            options.reserve(opts.size());
            for (const Item* it : opts) options.push_back(it->getDisplayName());
            std::vector<int> selected;
            for (const auto& id : ids) {
                for (int i = 0; i < static_cast<int>(opts.size()); ++i) {
                    if (opts[i]->getItemId() == id) { selected.push_back(i); break; }
                }
            }
            return Inspector::SelectValue(selected, options);
        }

        // Effect sub-fields
        auto effectField = [](const Data::Effect& e, const std::string& id) -> std::optional<int> {
            if (id.ends_with("AppliedTo"))  return static_cast<int>(e.appliedTo);
            if (id.ends_with("Base"))       return static_cast<int>(e.base);
            if (id.ends_with("CritChance")) return static_cast<int>(e.critChance);
            if (id.ends_with("CritMult"))   return static_cast<int>(e.critMultiplier);
            if (id.ends_with("Rate"))       return static_cast<int>(e.rate);
            if (id.ends_with("Unit"))       return static_cast<int>(e.unit);
            return std::nullopt;
        };
        if (propertyId.starts_with("dmg")) {
            if (auto v = effectField(m_data->getDamageEffect(), propertyId)) return *v;
        }
        if (propertyId.starts_with("heal")) {
            if (auto v = effectField(m_data->getHealEffect(), propertyId)) return *v;
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
     * @version Aug 2026
     *
     * @param propertyId The unique property identifier string
     * @param value The new value (must match the property's expected type)
     * @return bool True if the value was accepted and written, false otherwise
     */
    bool Item::setPropertyValue(
        const std::string& propertyId,
        const Inspector::PropertyValue& value
    ) {
        auto setDescriptionSlot = [this](Data::LocalizedText Data::DescriptionTexts::* slot,
                                         const Inspector::LocalizedText& text) {
            Data::DescriptionTexts texts = m_data->getDescriptionTexts();
            texts.*slot = text;
            setDescriptionTexts(texts);
        };
        auto setEffectField = [](Data::Effect& e, const std::string& id, int raw) {
            const auto v = static_cast<uint8_t>(raw);
            if (id.ends_with("AppliedTo"))  e.appliedTo = v;
            else if (id.ends_with("Base"))       e.base = v;
            else if (id.ends_with("CritChance")) e.critChance = v;
            else if (id.ends_with("CritMult"))   e.critMultiplier = v;
            else if (id.ends_with("Rate"))       e.rate = v;
            else if (id.ends_with("Unit"))       e.unit = v;
        };

        if (propertyId == "name") {
            if (auto* str = std::get_if<std::string>(&value)) { setName(*str); return true; }
        }
        else if (propertyId == "isContainer") {
            if (auto* b = std::get_if<bool>(&value)) { setContainer(*b); return true; }
        }
        else if (propertyId == "weight") {
            if (auto* i = std::get_if<int>(&value)) { setWeight(static_cast<uint8_t>(*i)); return true; }
        }
        else if (propertyId == "slots") {
            if (auto* i = std::get_if<int>(&value)) { setSlots(static_cast<uint8_t>(*i)); return true; }
        }
        else if (propertyId == "serviceLife") {
            if (auto* i = std::get_if<int>(&value)) { setServiceLife(static_cast<uint8_t>(*i)); return true; }
        }
        else if (propertyId == "itemType") {
            if (auto* ev = std::get_if<Inspector::EnumValue>(&value)) { setItemType(ev->selectedIndex); return true; }
        }
        else if (propertyId == "imagePath") {
            if (auto* str = std::get_if<std::string>(&value)) { setImagePath(*str); return true; }
        }
        else if (propertyId == "synonyms") {
            if (auto* str = std::get_if<std::string>(&value)) { setSynonyms(splitLines(*str)); return true; }
        }
        else if (propertyId == "abbreviatures") {
            if (auto* str = std::get_if<std::string>(&value)) { setAbbreviatures(splitLines(*str)); return true; }
        }
        else if (propertyId == "descriptionsNormal") {
            if (auto* t = std::get_if<Inspector::LocalizedText>(&value)) { setDescriptionSlot(&Data::DescriptionTexts::normal, *t); return true; }
        }
        else if (propertyId == "descriptionsLongText") {
            if (auto* t = std::get_if<Inspector::LocalizedText>(&value)) { setDescriptionSlot(&Data::DescriptionTexts::longText, *t); return true; }
        }
        else if (propertyId == "descriptionsOdor") {
            if (auto* t = std::get_if<Inspector::LocalizedText>(&value)) { setDescriptionSlot(&Data::DescriptionTexts::odor, *t); return true; }
        }
        else if (propertyId == "descriptionsSound") {
            if (auto* t = std::get_if<Inspector::LocalizedText>(&value)) { setDescriptionSlot(&Data::DescriptionTexts::sound, *t); return true; }
        }
        else if (propertyId == "affordances") {
            if (auto* list = std::get_if<Inspector::AffordanceListValue>(&value)) {
                std::vector<Data::Affordance> affordances;
                affordances.reserve(list->size());
                for (const auto& e : *list) {
                    affordances.push_back({e.name, e.triggers, e.presetKey});
                }
                setAffordances(affordances);
                return true;
            }
        }
        else if (propertyId == "containerItems" || propertyId == "combinableWith") {
            if (auto* sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::vector<ADS::Types::ObjectId> ids;
                const std::vector<Item*> opts = relationItemOptions();
                for (int idx : sel->selectedIndices) {
                    if (idx >= 0 && idx < static_cast<int>(opts.size())) {
                        ids.push_back(opts[idx]->getItemId());
                    }
                }
                if (propertyId == "containerItems") setContainerItemIds(ids);
                else setCombinableWithIds(ids);
                return true;
            }
        }
        else if (propertyId == "state") {
            if (auto* sel = std::get_if<Inspector::SelectValue>(&value)) {
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
        }
        else if (propertyId == "initialSceneId") {
            if (auto* sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::optional<ADS::Types::SceneId> newTarget;
                if (!sel->selectedIndices.empty() && getProject()) {
                    const auto& scenes = getProject()->getScenes();
                    int idx = sel->selectedIndices.front();
                    if (idx >= 0 && idx < static_cast<int>(scenes.size())) {
                        newTarget = scenes[idx]->getSceneId();
                    }
                }
                setInitialSceneId(newTarget);
                return true;
            }
        }
        else if (propertyId.starts_with("dmg")) {
            if (auto* i = std::get_if<int>(&value)) {
                Data::Effect e = m_data->getDamageEffect();
                setEffectField(e, propertyId, *i);
                setDamageEffect(e);
                return true;
            }
        }
        else if (propertyId.starts_with("heal")) {
            if (auto* i = std::get_if<int>(&value)) {
                Data::Effect e = m_data->getHealEffect();
                setEffectField(e, propertyId, *i);
                setHealEffect(e);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Build the option-label list for every state in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::vector<std::string> One label per state, in project order
     */
    std::vector<std::string> Item::buildStateOptionLabels() const {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& state : getProject()->getStates()) {
            labels.push_back(state->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Build the option-label list for every scene in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Used by the "initial scene" dropdown. Mirrors
     * Entities::Scene::buildSceneOptionLabels().
     *
     * @return std::vector<std::string> One label per scene, in project order
     */
    std::vector<std::string> Item::buildSceneOptionLabels() const {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& scene : getProject()->getScenes()) {
            labels.push_back(scene->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Build the option-label list for every item in the project
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::vector<std::string> One label per item, in project order
     */
    std::vector<std::string> Item::buildItemOptionLabels() const {
        std::vector<std::string> labels;
        for (const Item* item : relationItemOptions()) {
            labels.push_back(item->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Project items eligible for the container / combinable lists
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return std::vector<Item*> Every project item except this one
     */
    std::vector<Item*> Item::relationItemOptions() const {
        std::vector<Item*> out;
        if (!getProject()) return out;
        for (const auto& item : getProject()->getItems()) {
            if (item->getItemId() == getItemId()) {
                continue;  // an item can't contain / combine with itself
            }
            out.push_back(item.get());
        }
        return out;
    }

    /**
     * @brief Build the preset list of known affordance names
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return std::vector<std::string> Known affordance names
     */
    std::vector<std::string> Item::buildKnownAffordanceNames() {
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
     * Index-paired with buildKnownAffordanceNames() — element i here is
     * the untranslated key behind element i's translated label, so a
     * picked preset can be re-translated live later (see
     * Data::Affordance::presetKey).
     *
     * @return std::vector<std::string> Known affordance translation keys
     */
    std::vector<std::string> Item::buildKnownAffordancePresetKeys() {
        std::vector<std::string> keys;
        for (const char* key : knownAffordanceKeys()) {
            keys.emplace_back(key);
        }
        return keys;
    }

    /**
     * @brief Get the item's sensory descriptions (LexEngine text ids)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::Descriptions& LexEngine text-id references
     */
    const Data::Descriptions& Item::getDescriptions() const {
        return m_data->getDescriptions();
    }

    /**
     * @brief Set the item's sensory descriptions (LexEngine text ids)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param descriptions The new descriptions
     */
    void Item::setDescriptions(const Data::Descriptions& descriptions) {
        if (m_data->getDescriptions() != descriptions) {
            m_data->setDescriptions(descriptions);
            notifyPropertyChanged("descriptions", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the author-typed draft text backing the descriptions group
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::DescriptionTexts& Draft description text
     */
    const Data::DescriptionTexts& Item::getDescriptionTexts() const {
        return m_data->getDescriptionTexts();
    }

    /**
     * @brief Set the author-typed draft text backing the descriptions group
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param texts The new draft description text
     */
    void Item::setDescriptionTexts(const Data::DescriptionTexts& texts) {
        if (m_data->getDescriptionTexts() != texts) {
            m_data->setDescriptionTexts(texts);
            notifyPropertyChanged("descriptionTexts", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the item's affordances
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return const std::vector<Data::Affordance>& The item's affordances
     */
    const std::vector<Data::Affordance>& Item::getAffordances() const {
        return m_data->getAffordances();
    }

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
    void Item::setAffordances(const std::vector<Data::Affordance>& affordances) {
        if (m_data->getAffordances() != affordances) {
            m_data->setAffordances(affordances);
            notifyPropertyChanged("affordances", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Check whether the item is marked as a container
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * When false the inspector hides the contained-items list.
     *
     * @return bool True if the item is a container
     */
    bool Item::isContainer() const {
        return m_data->isContainer();
    }

    /**
     * @brief Mark or unmark the item as a container
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Fires a property-changed event if the value actually changed.
     * Existing contained-items are left intact so re-enabling the flag
     * restores them.
     *
     * The flag is linked to the item type: setting type Container marks this
     * (see setItemType()), and clearing it while the type is still Container
     * demotes the type to Generic so the two never contradict. Marking it
     * does **not** force the type — any item can be a container.
     *
     * @param container True to mark the item as a container
     */
    void Item::setContainer(bool container) {
        if (m_data->isContainer() != container) {
            m_data->setContainer(container);
            notifyPropertyChanged("isContainer", !container, container);
        }

        if (!container && m_data->getItemType() == kContainerTypeIndex) {
            setItemType(0);  // Generic — a non-container of type "Container" is contradictory
        }
    }

    /**
     * @brief Get the IDs of items held inside this item
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::vector<ADS::Types::ObjectId>& Contained item IDs
     */
    const std::vector<ADS::Types::ObjectId>& Item::getContainerItemIds() const {
        return m_data->getContainerItemIds();
    }

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
    void Item::setContainerItemIds(const std::vector<ADS::Types::ObjectId>& ids) {
        if (m_data->getContainerItemIds() != ids) {
            m_data->setContainerItemIds(ids);
            notifyPropertyChanged("containerItems", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the IDs of items this item can be combined with
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::vector<ADS::Types::ObjectId>& Combinable item IDs
     */
    const std::vector<ADS::Types::ObjectId>& Item::getCombinableWithIds() const {
        return m_data->getCombinableWithIds();
    }

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
    void Item::setCombinableWithIds(const std::vector<ADS::Types::ObjectId>& ids) {
        if (m_data->getCombinableWithIds() != ids) {
            m_data->setCombinableWithIds(ids);
            notifyPropertyChanged("combinableWith", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the item's synonyms
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::vector<std::string>& Synonym words
     */
    const std::vector<std::string>& Item::getSynonyms() const {
        return m_data->getSynonyms();
    }

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
    void Item::setSynonyms(const std::vector<std::string>& synonyms) {
        if (m_data->getSynonyms() != synonyms) {
            m_data->setSynonyms(synonyms);
            notifyPropertyChanged("synonyms", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the item's abbreviations
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::vector<std::string>& Abbreviation words
     */
    const std::vector<std::string>& Item::getAbbreviatures() const {
        return m_data->getAbbreviatures();
    }

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
    void Item::setAbbreviatures(const std::vector<std::string>& abbreviatures) {
        if (m_data->getAbbreviatures() != abbreviatures) {
            m_data->setAbbreviatures(abbreviatures);
            notifyPropertyChanged("abbreviatures", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the damage effect this item applies when used
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::Effect& The damage effect
     */
    const Data::Effect& Item::getDamageEffect() const {
        return m_data->getDamageEffect();
    }

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
    void Item::setDamageEffect(const Data::Effect& effect) {
        if (!(m_data->getDamageEffect() == effect)) {
            m_data->setDamageEffect(effect);
            notifyPropertyChanged("damageEffect", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the heal effect this item applies when used
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::Effect& The heal effect
     */
    const Data::Effect& Item::getHealEffect() const {
        return m_data->getHealEffect();
    }

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
    void Item::setHealEffect(const Data::Effect& effect) {
        if (!(m_data->getHealEffect() == effect)) {
            m_data->setHealEffect(effect);
            notifyPropertyChanged("healEffect", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the item's weight
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return uint8_t Weight, 0 (lightest) to 255 (heaviest)
     */
    uint8_t Item::getWeight() const {
        return m_data->getWeight();
    }

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
    void Item::setWeight(uint8_t weight) {
        setAndNotify("weight",
            [this]{ return m_data->getWeight(); },
            [this](uint8_t v){ m_data->setWeight(v); },
            weight);
    }

    /**
     * @brief Get the number of inventory slots this item occupies
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return uint8_t Slot count
     */
    uint8_t Item::getSlots() const {
        return m_data->getSlots();
    }

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
    void Item::setSlots(uint8_t slots) {
        setAndNotify("slots",
            [this]{ return m_data->getSlots(); },
            [this](uint8_t v){ m_data->setSlots(v); },
            slots);
    }

    /**
     * @brief Get the item's remaining service life
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return uint8_t Remaining service life, 0-255
     */
    uint8_t Item::getServiceLife() const {
        return m_data->getServiceLife();
    }

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
    void Item::setServiceLife(uint8_t serviceLife) {
        setAndNotify("serviceLife",
            [this]{ return m_data->getServiceLife(); },
            [this](uint8_t v){ m_data->setServiceLife(v); },
            serviceLife);
    }

    /**
     * @brief Get the item type index
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return int Index into the getItemTypes() list
     */
    int Item::getItemType() const {
        return m_data->getItemType();
    }

    /**
     * @brief Set the item type index
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Fires a property-changed event (as EnumValue) if the value actually changed.
     *
     * Type Container implies isContainer(), so selecting it also marks the
     * flag. The reverse is not forced — see setContainer().
     *
     * @param type Index into the getItemTypes() list
     */
    void Item::setItemType(int type) {
        if (m_data->getItemType() != type) {
            int oldType = m_data->getItemType();
            m_data->setItemType(type);
            notifyPropertyChanged("itemType",
                Inspector::EnumValue(oldType, getItemTypes()),
                Inspector::EnumValue(m_data->getItemType(), getItemTypes()));
        }
        if (type == kContainerTypeIndex && !isContainer()) {
            setContainer(true);
        }
    }

    /**
     * @brief Get the path to the item image
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::string& File path to the image
     */
    const std::string& Item::getImagePath() const {
        return m_data->getImagePath();
    }

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
    void Item::setImagePath(const std::string& path) {
        setAndNotify("imagePath",
            [this]{ return m_data->getImagePath(); },
            [this](const std::string& v){ m_data->setImagePath(v); },
            path);
    }

    /**
     * @brief Get the ID of the scene where this item starts
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::optional<ADS::Types::SceneId>& Starting scene ID
     */
    const std::optional<ADS::Types::SceneId>& Item::getInitialSceneId() const {
        return m_data->getInitialSceneId();
    }

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
    void Item::setInitialSceneId(const std::optional<ADS::Types::SceneId>& sceneId) {
        if (m_data->getInitialSceneId() != sceneId) {
            auto toStr = [](const std::optional<ADS::Types::SceneId>& id) {
                return id.has_value() ? std::to_string(id->value) : std::string();
            };
            std::string oldStr = toStr(m_data->getInitialSceneId());
            m_data->setInitialSceneId(sceneId);
            notifyPropertyChanged("initialSceneId", oldStr, toStr(sceneId));
        }
    }

    /**
     * @brief Get the item's current state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
     */
    const std::optional<ADS::Types::StateId>& Item::getState() const {
        return m_data->getState();
    }

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
    void Item::setState(const std::optional<ADS::Types::StateId>& state) {
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
     * @brief Get the item type name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Human-readable type name for the current item type index
     */
    std::string Item::getItemTypeName() const {
        const auto& types = getItemTypes();
        const int type = m_data->getItemType();
        if (type >= 0 && type < static_cast<int>(types.size())) {
            return types[type];
        }
        return translate("ITEM.TYPE_UNKNOWN");
    }
}
