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
 * @file Character.cpp
 * @brief Implementation of the Character entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include "Character.h"
#include "Core/Project.h"
#include "image_formats.h"
#include "imgui.h"

namespace {
    /// Index of the state with the given id within project->getStates(), or
    /// -1 if absent/unresolvable. Must iterate in the same order as
    /// Character::buildStateOptionLabels() so indices line up. Mirrors the
    /// same helper in Entities::Scene and Entities::Item.
    int findStateOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::StateId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& states = project->getStates();
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->getStateId() == *id) return static_cast<int>(i);
        }
        return -1;
    }

    /// Index of the scene with the given id within project->getScenes(), or
    /// -1 if absent/unresolvable. Must iterate in the same order as
    /// Character::buildSceneOptionLabels(). Mirrors Scene/Item.
    int findSceneOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::SceneId>& id) {
        if (!project || !id.has_value()) return -1;
        const auto& scenes = project->getScenes();
        for (size_t i = 0; i < scenes.size(); ++i) {
            if (scenes[i]->getSceneId() == *id) return static_cast<int>(i);
        }
        return -1;
    }

    /// One entry per character-applicable global trigger key
    /// (common-structures.md's Global triggers table). Options are always
    /// empty for now — no Event DataObject/catalog exists yet — forward-wired
    /// so this lights up automatically once one does, without any editor
    /// rework. Mirrors Scene's/Item's own triggerKeys().
    struct TriggerKeyInfo {
        const char* propertyId;
        const char* translationKey;
        uint8_t triggerId;
    };

    const std::vector<TriggerKeyInfo>& triggerKeys() {
        static const std::vector<TriggerKeyInfo> keys = {
            {"triggerOnTalk", "CHARACTER.TRIGGER_ON_TALK", 0x08},
            {"triggerOnDie",  "CHARACTER.TRIGGER_ON_DIE",  0x09},
            {"triggerOnHeal", "CHARACTER.TRIGGER_ON_HEAL", 0x0A},
            {"triggerOnHurt", "CHARACTER.TRIGGER_ON_HURT", 0x0B},
        };
        return keys;
    }
}

namespace ADS::Entities {
    /**
     * @brief Construct a new Character backed by the given CharacterData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param data Non-owning pointer to the CharacterData struct. Must not be
     *             null and must outlive this entity.
     */
    Character::Character(Data::CharacterData* data)
        : BaseEntity(data), m_data(data) {
    }

    /**
     * @brief Get the typed character identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return ADS::Types::CharacterId The character's typed identifier
     */
    ADS::Types::CharacterId Character::getCharacterId() const {
        return m_data->getId();
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Translated "Character" type name
     */
    std::string Character::getTypeName() const {
        return translate("CHARACTER.TYPE_NAME");
    }

    /**
     * @brief Get the list of property descriptors for this character
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
     */
    std::vector<Inspector::PropertyDescriptor> Character::getPropertyDescriptors() const {
        using namespace Inspector;

        std::vector<PropertyDescriptor> descriptors = {
            // Info category (read-only) — pinned above every other category
            // by InspectorPanel regardless of position here.
            PropertyDescriptor("id", translate("CHARACTER.PROP_ID"), PropertyType::String)
                .setCategory(translate("CATEGORY.INFO"))
                .setDescription(translate("CHARACTER.DESC_ID"))
                .setReadOnly(),

            // General category
            PropertyDescriptor("name", translate("CHARACTER.PROP_NAME"), PropertyType::String)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("CHARACTER.DESC_NAME"))
                .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("isPlayer", translate("CHARACTER.PROP_IS_PLAYER"), PropertyType::Bool)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("CHARACTER.DESC_IS_PLAYER")),

            PropertyDescriptor("initialSceneId", translate("CHARACTER.PROP_INITIAL_SCENE"), PropertyType::Select)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("CHARACTER.DESC_INITIAL_SCENE"))
                .setOptionsProvider([this] { return buildSceneOptionLabels(); }),

            PropertyDescriptor("state", translate("CHARACTER.PROP_STATE"), PropertyType::Select)
                .setCategory(translate("CATEGORY.GENERAL"))
                .setDescription(translate("CHARACTER.DESC_STATE"))
                .setOptionsProvider([this] { return buildStateOptionLabels(); })
                .setEmptyOptionsText("INSPECTOR.NO_STATES")
                .setAllowCreateNew(),

            // Affordances category — editable list of named affordances,
            // each with its own trigger-name list.
            PropertyDescriptor("affordances", translate("CHARACTER.PROP_AFFORDANCES"), PropertyType::AffordanceList)
                .setCategory(translate("CATEGORY.AFFORDANCES"))
                .setDescription(translate("CHARACTER.DESC_AFFORDANCES")),

            // Capacities category (character.md: capacities.load + vitalities)
            PropertyDescriptor("capLoad", translate("CHARACTER.PROP_LOAD"), PropertyType::Int)
                .setCategory(translate("CATEGORY.CAPACITIES"))
                .setDescription(translate("CHARACTER.DESC_LOAD"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            PropertyDescriptor("capLife", translate("CHARACTER.PROP_LIFE"), PropertyType::Int)
                .setCategory(translate("CATEGORY.CAPACITIES"))
                .setDescription(translate("CHARACTER.DESC_LIFE"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            PropertyDescriptor("capStamina", translate("CHARACTER.PROP_STAMINA"), PropertyType::Int)
                .setCategory(translate("CATEGORY.CAPACITIES"))
                .setDescription(translate("CHARACTER.DESC_STAMINA"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            PropertyDescriptor("capSanity", translate("CHARACTER.PROP_SANITY"), PropertyType::Int)
                .setCategory(translate("CATEGORY.CAPACITIES"))
                .setDescription(translate("CHARACTER.DESC_SANITY"))
                .setConstraints(PropertyConstraints::numeric(0, 255, 1)),

            // Appearance category
            PropertyDescriptor("imagePath", translate("CHARACTER.PROP_IMAGE"), PropertyType::String)
                .setCategory(translate("CATEGORY.APPEARANCE"))
                .setDescription(translate("CHARACTER.DESC_IMAGE"))
                .setConstraints(PropertyConstraints::filePath(
                    ADS::Constants::ImageFormats::supportedExtensions())),

            PropertyDescriptor("avatarPath", translate("CHARACTER.PROP_AVATAR"), PropertyType::String)
                .setCategory(translate("CATEGORY.APPEARANCE"))
                .setDescription(translate("CHARACTER.DESC_AVATAR"))
                .setConstraints(PropertyConstraints::filePath(
                    ADS::Constants::ImageFormats::supportedExtensions())),

            PropertyDescriptor("dialogColor", translate("CHARACTER.PROP_DIALOG_COLOR"), PropertyType::Color)
                .setCategory(translate("CATEGORY.APPEARANCE"))
                .setDescription(translate("CHARACTER.DESC_DIALOG_COLOR")),

            // Descriptions category — author-typed draft text
            PropertyDescriptor("descriptionsNormal", translate("CHARACTER.PROP_DESC_NORMAL"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("CHARACTER.DESC_DESC_NORMAL"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsLongText", translate("CHARACTER.PROP_DESC_LONG"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("CHARACTER.DESC_DESC_LONG"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsOdor", translate("CHARACTER.PROP_DESC_ODOR"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("CHARACTER.DESC_DESC_ODOR"))
                .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsSound", translate("CHARACTER.PROP_DESC_SOUND"), PropertyType::String)
                .setCategory(translate("CATEGORY.DESCRIPTIONS"))
                .setDescription(translate("CHARACTER.DESC_DESC_SOUND"))
                .setConstraints(PropertyConstraints::translatableText(512)),
        };

        // Triggers category — one multi-select property per
        // character-applicable global trigger key; always shows "No options
        // available" until an Event DataObject/catalog exists (see
        // triggerKeys() docs above).
        for (const auto& key : triggerKeys()) {
            descriptors.push_back(
                PropertyDescriptor(key.propertyId, translate(key.translationKey), PropertyType::Select)
                .setCategory(translate("CATEGORY.TRIGGERS"))
                .setDescription(translate("CHARACTER.DESC_TRIGGER"))
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
    Inspector::PropertyValue Character::getPropertyValue(const std::string& propertyId) const {
        if (propertyId == "name")        return m_data->getName();
        if (propertyId == "isPlayer")    return m_data->isPlayer();
        if (propertyId == "capLoad")     return static_cast<int>(m_data->getCapacities().load);
        if (propertyId == "capLife")     return static_cast<int>(m_data->getCapacities().life);
        if (propertyId == "capStamina")  return static_cast<int>(m_data->getCapacities().stamina);
        if (propertyId == "capSanity")   return static_cast<int>(m_data->getCapacities().sanity);
        if (propertyId == "imagePath")   return m_data->getImagePath();
        if (propertyId == "avatarPath")  return m_data->getAvatarPath();
        if (propertyId == "dialogColor") {
            const auto& c = m_data->getDialogColor();
            return ImVec4(c.r, c.g, c.b, c.a);
        }
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
        if (propertyId == "state") {
            std::vector<std::string> options = buildStateOptionLabels();
            std::vector<int> selected;
            int idx = findStateOptionIndex(getProject(), m_data->getState());
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }
        if (propertyId == "id")          return std::to_string(m_data->getId().value);
        if (propertyId == "affordances") {
            Inspector::AffordanceListValue value;
            for (const auto& a : m_data->getAffordances()) {
                const std::string displayName = a.presetKey.empty() ? a.name : translate(a.presetKey);
                value.emplace_back(displayName, a.triggers, a.presetKey);
            }
            return value;
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
     * Validates the type of @p value against the expected type for
     * @p propertyId before writing. Fires a property-changed event on success.
     *
     * @param propertyId The unique property identifier string
     * @param value The new value (must match the property's expected type)
     * @return bool True if the value was accepted and written, false otherwise
     */
    bool Character::setPropertyValue(
        const std::string& propertyId,
        const Inspector::PropertyValue& value
    ) {
        auto setCapacity = [this](uint8_t Data::Capacities::* member, int raw) {
            Data::Capacities caps = m_data->getCapacities();
            caps.*member = static_cast<uint8_t>(raw);
            setCapacities(caps);
        };

        if (propertyId == "name") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setName(*str);
                return true;
            }
        }
        else if (propertyId == "isPlayer") {
            if (auto* b = std::get_if<bool>(&value)) {
                setPlayer(*b);
                return true;
            }
        }
        else if (propertyId == "capLoad") {
            if (auto* i = std::get_if<int>(&value)) { setCapacity(&Data::Capacities::load, *i); return true; }
        }
        else if (propertyId == "capLife") {
            if (auto* i = std::get_if<int>(&value)) { setCapacity(&Data::Capacities::life, *i); return true; }
        }
        else if (propertyId == "capStamina") {
            if (auto* i = std::get_if<int>(&value)) { setCapacity(&Data::Capacities::stamina, *i); return true; }
        }
        else if (propertyId == "capSanity") {
            if (auto* i = std::get_if<int>(&value)) { setCapacity(&Data::Capacities::sanity, *i); return true; }
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
        else if (propertyId == "dialogColor") {
            if (auto* color = std::get_if<ImVec4>(&value)) {
                setDialogColor({color->x, color->y, color->z, color->w});
                return true;
            }
        }
        else if (propertyId == "imagePath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setImagePath(*str);
                return true;
            }
        }
        else if (propertyId == "avatarPath") {
            if (auto* str = std::get_if<std::string>(&value)) {
                setAvatarPath(*str);
                return true;
            }
        }
        else if (propertyId == "descriptionsNormal") {
            if (auto* localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.normal = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        }
        else if (propertyId == "descriptionsLongText") {
            if (auto* localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.longText = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        }
        else if (propertyId == "descriptionsOdor") {
            if (auto* localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.odor = *localized;
                setDescriptionTexts(texts);
                return true;
            }
        }
        else if (propertyId == "descriptionsSound") {
            if (auto* localized = std::get_if<Inspector::LocalizedText>(&value)) {
                Data::DescriptionTexts texts = m_data->getDescriptionTexts();
                texts.sound = *localized;
                setDescriptionTexts(texts);
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
    std::vector<std::string> Character::buildStateOptionLabels() const {
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
     * @return std::vector<std::string> One label per scene, in project order
     */
    std::vector<std::string> Character::buildSceneOptionLabels() const {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (const auto& scene : getProject()->getScenes()) {
            labels.push_back(scene->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Get the character's sensory descriptions (LexEngine text ids)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::Descriptions& LexEngine text-id references
     */
    const Data::Descriptions& Character::getDescriptions() const {
        return m_data->getDescriptions();
    }

    /**
     * @brief Set the character's sensory descriptions (LexEngine text ids)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param descriptions The new descriptions
     */
    void Character::setDescriptions(const Data::Descriptions& descriptions) {
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
    const Data::DescriptionTexts& Character::getDescriptionTexts() const {
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
    void Character::setDescriptionTexts(const Data::DescriptionTexts& texts) {
        if (m_data->getDescriptionTexts() != texts) {
            m_data->setDescriptionTexts(texts);
            notifyPropertyChanged("descriptionTexts", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the character's capacities (load + life/stamina/sanity)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const Data::Capacities& Current capacity values
     */
    const Data::Capacities& Character::getCapacities() const {
        return m_data->getCapacities();
    }

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
    void Character::setCapacities(const Data::Capacities& capacities) {
        if (m_data->getCapacities() != capacities) {
            m_data->setCapacities(capacities);
            notifyPropertyChanged("capacities", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Check whether this character is the player character
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return bool True if this is the player-controlled character
     */
    bool Character::isPlayer() const {
        return m_data->isPlayer();
    }

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
    void Character::setPlayer(bool isPlayer) {
        setAndNotify("isPlayer",
            [this]{ return m_data->isPlayer(); },
            [this](bool v){ m_data->setPlayer(v); },
            isPlayer);
    }

    /**
     * @brief Get the character's current state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::optional<ADS::Types::StateId>& Current state id, or std::nullopt
     */
    const std::optional<ADS::Types::StateId>& Character::getState() const {
        return m_data->getState();
    }

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
    void Character::setState(const std::optional<ADS::Types::StateId>& state) {
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
     * @brief Get the color used for this character's dialog text
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const ADS::Types::Color& Dialog text color
     */
    const ADS::Types::Color& Character::getDialogColor() const {
        return m_data->getDialogColor();
    }

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
    void Character::setDialogColor(const ADS::Types::Color& color) {
        if (m_data->getDialogColor() != color) {
            const auto& old = m_data->getDialogColor();
            ImVec4 oldVec(old.r, old.g, old.b, old.a);
            m_data->setDialogColor(color);
            ImVec4 newVec(color.r, color.g, color.b, color.a);
            notifyPropertyChanged("dialogColor", oldVec, newVec);
        }
    }

    /**
     * @brief Get the path to the character's main (full-body) image
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::string& File path to the main image
     */
    const std::string& Character::getImagePath() const {
        return m_data->getImagePath();
    }

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
    void Character::setImagePath(const std::string& path) {
        setAndNotify("imagePath",
            [this]{ return m_data->getImagePath(); },
            [this](const std::string& v){ m_data->setImagePath(v); },
            path);
    }

    /**
     * @brief Get the path to the character's avatar image
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return const std::string& File path to the avatar image
     */
    const std::string& Character::getAvatarPath() const {
        return m_data->getAvatarPath();
    }

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
    void Character::setAvatarPath(const std::string& path) {
        setAndNotify("avatarPath",
            [this]{ return m_data->getAvatarPath(); },
            [this](const std::string& v){ m_data->setAvatarPath(v); },
            path);
    }

    /**
     * @brief Get the ID of the scene where this character starts
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return const std::optional<ADS::Types::SceneId>& Starting scene ID
     */
    const std::optional<ADS::Types::SceneId>& Character::getInitialSceneId() const {
        return m_data->getInitialSceneId();
    }

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
    void Character::setInitialSceneId(const std::optional<ADS::Types::SceneId>& sceneId) {
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
     * @brief Get the character's affordances
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @return const std::vector<Data::Affordance>& The character's affordances
     */
    const std::vector<Data::Affordance>& Character::getAffordances() const {
        return m_data->getAffordances();
    }

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
    void Character::setAffordances(const std::vector<Data::Affordance>& affordances) {
        if (m_data->getAffordances() != affordances) {
            m_data->setAffordances(affordances);
            notifyPropertyChanged("affordances", std::monostate{}, std::monostate{});
        }
    }

}
