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
 * @file State.cpp
 * @brief Implementation of the State entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include "State.h"
#include "Core/Project.h"

namespace {
    /// Index of the state with the given id within the free-states list
    /// (project->getFreeStates(excludeSelf, id) — id itself is kept visible
    /// even though it's "used" by this very field), or -1 if
    /// absent/unresolvable. Must iterate in the same order as
    /// State::buildStateOptionLabels() so indices line up.
    int findStateOptionIndex(
        ADS::Core::Project* project,
        const ADS::Types::StateId& excludeSelf,
        const std::optional<ADS::Types::StateId>& id
    ) {
        if (!project || !id.has_value()) return -1;
        auto states = project->getFreeStates(excludeSelf, id);
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->getStateId() == *id) return static_cast<int>(i);
        }
        return -1;
    }
}

namespace ADS::Entities {
    /**
     * @brief Construct a new State backed by the given StateData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param data Non-owning pointer to the StateData struct. Must not be
     *             null and must outlive this entity.
     */
    State::State(Data::StateData *data) :
        BaseEntity(data), m_data(data)
    {
    }

    /**
     * @brief Get the typed state identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return ADS::Types::StateId The state's typed identifier
     */
    ADS::Types::StateId State::getStateId() const
    {
        return m_data->getId();
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::string Translated "State" type name
     */
    std::string State::getTypeName() const
    {
        return translate("STATE.TYPE_NAME");
    }

    /**
     * @brief Get the list of property descriptors for this state
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
     */
    std::vector<Inspector::PropertyDescriptor> State::getPropertyDescriptors() const
    {
        using namespace Inspector;

        return {
            // Info category (read-only)
            PropertyDescriptor("id", translate("STATE.PROP_ID"), PropertyType::String)
            .setCategory(translate("CATEGORY.INFO"))
            .setDescription(translate("STATE.DESC_ID"))
            .setReadOnly(),

            // General category
            PropertyDescriptor("name", translate("STATE.PROP_NAME"), PropertyType::String)
            .setCategory(translate("CATEGORY.GENERAL"))
            .setDescription(translate("STATE.DESC_NAME"))
            .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("next", translate("STATE.PROP_NEXT"), PropertyType::Select)
            .setCategory(translate("CATEGORY.GENERAL"))
            .setDescription(translate("STATE.DESC_NEXT"))
            .setOptionsProvider([this] { return buildStateOptionLabels(); })
            .setEmptyOptionsText("INSPECTOR.NO_STATES")
            .setAllowCreateNew(),

            // Descriptions category — author-typed draft text; separate
            // from the (not-yet-compiled) LexEngine ids on Descriptions.
            PropertyDescriptor("descriptionsNormal", translate("STATE.PROP_DESC_NORMAL"), PropertyType::String)
            .setCategory(translate("CATEGORY.DESCRIPTIONS"))
            .setDescription(translate("STATE.DESC_DESC_NORMAL"))
            .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsLongText", translate("STATE.PROP_DESC_LONG"), PropertyType::String)
            .setCategory(translate("CATEGORY.DESCRIPTIONS"))
            .setDescription(translate("STATE.DESC_DESC_LONG"))
            .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsOdor", translate("STATE.PROP_DESC_ODOR"), PropertyType::String)
            .setCategory(translate("CATEGORY.DESCRIPTIONS"))
            .setDescription(translate("STATE.DESC_DESC_ODOR"))
            .setConstraints(PropertyConstraints::translatableText(512)),

            PropertyDescriptor("descriptionsSound", translate("STATE.PROP_DESC_SOUND"), PropertyType::String)
            .setCategory(translate("CATEGORY.DESCRIPTIONS"))
            .setDescription(translate("STATE.DESC_DESC_SOUND"))
            .setConstraints(PropertyConstraints::translatableText(512)),
        };
    }

    /**
     * @brief Get the current value of a property by ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param propertyId The unique property identifier string
     * @return Inspector::PropertyValue Current value, or std::monostate if unknown
     */
    Inspector::PropertyValue State::getPropertyValue(const std::string &propertyId) const
    {
        if (propertyId == "id")
            return std::to_string(m_data->getId().value);
        if (propertyId == "name")
            return m_data->getName();
        if (propertyId == "descriptionsNormal")
            return m_data->getDescriptionTexts().normal;
        if (propertyId == "descriptionsLongText")
            return m_data->getDescriptionTexts().longText;
        if (propertyId == "descriptionsOdor")
            return m_data->getDescriptionTexts().odor;
        if (propertyId == "descriptionsSound")
            return m_data->getDescriptionTexts().sound;
        if (propertyId == "next") {
            std::vector<std::string> options = buildStateOptionLabels();
            std::vector<int> selected;
            int idx = findStateOptionIndex(getProject(), getStateId(), m_data->getNext());
            if (idx >= 0) selected.push_back(idx);
            return Inspector::SelectValue(selected, options);
        }

        return std::monostate{};
    }

    /**
     * @brief Set the value of a property by ID
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Validates the type of @p value against the expected type for
     * @p propertyId before writing. Fires a property-changed event on success.
     *
     * @param propertyId The unique property identifier string
     * @param value The new value (must match the property's expected type)
     * @return bool True if the value was accepted and written, false otherwise
     */
    bool State::setPropertyValue(const std::string &propertyId, const Inspector::PropertyValue &value)
    {
        if (propertyId == "name") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setName(*str);
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
        } else if (propertyId == "next") {
            if (auto *sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::optional<ADS::Types::StateId> newTarget;
                if (!sel->selectedIndices.empty() && getProject()) {
                    auto states = getProject()->getFreeStates(getStateId(), m_data->getNext());
                    int idx = sel->selectedIndices.front();
                    if (idx >= 0 && idx < static_cast<int>(states.size())) {
                        newTarget = states[idx]->getStateId();
                    }
                }
                setNext(newTarget);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Build the option-label list for states available as this state's "next"
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Only "free" states — this state itself and any state already claimed
     * as another state's next or a chain's head are excluded, except
     * whichever state this state's own next currently points to (kept
     * visible so it doesn't vanish from the dropdown). See
     * Core::Project::getFreeStates().
     *
     * @return std::vector<std::string> One label per selectable state
     */
    std::vector<std::string> State::buildStateOptionLabels() const
    {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (auto* state : getProject()->getFreeStates(getStateId(), m_data->getNext())) {
            labels.push_back(state->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Get the sensory descriptions shown while this state is active
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const Data::Descriptions& LexEngine text-id references for this state
     */
    const Data::Descriptions& State::getDescriptions() const
    {
        return m_data->getDescriptions();
    }

    /**
     * @brief Set the sensory descriptions shown while this state is active
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Fires a property-changed event if the value actually changed.
     *
     * @param descriptions The new descriptions
     */
    void State::setDescriptions(const Data::Descriptions &descriptions)
    {
        if (m_data->getDescriptions() != descriptions) {
            m_data->setDescriptions(descriptions);
            notifyPropertyChanged("descriptions", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the author-typed draft text for this state's descriptions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const Data::DescriptionTexts& Draft description text
     */
    const Data::DescriptionTexts& State::getDescriptionTexts() const
    {
        return m_data->getDescriptionTexts();
    }

    /**
     * @brief Set the author-typed draft text for this state's descriptions
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Fires a property-changed event if the value actually changed.
     *
     * @param texts The new draft description text
     */
    void State::setDescriptionTexts(const Data::DescriptionTexts &texts)
    {
        if (m_data->getDescriptionTexts() != texts) {
            m_data->setDescriptionTexts(texts);
            notifyPropertyChanged("descriptionTexts", std::monostate{}, std::monostate{});
        }
    }

    /**
     * @brief Get the next state in this state's chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::optional<ADS::Types::StateId>& Next state id, or
     *         std::nullopt if this is the last state in its chain
     */
    const std::optional<ADS::Types::StateId>& State::getNext() const
    {
        return m_data->getNext();
    }

    /**
     * @brief Set the next state in this state's chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Fires a property-changed event if the value actually changed.
     *
     * @param next The new next-state id, or std::nullopt to mark this as
     *             the last state in its chain
     */
    void State::setNext(const std::optional<ADS::Types::StateId> &next)
    {
        if (m_data->getNext() != next) {
            auto toStr = [](const std::optional<ADS::Types::StateId>& id) {
                return id.has_value() ? std::to_string(id->value) : std::string();
            };
            std::string oldStr = toStr(m_data->getNext());
            m_data->setNext(next);
            notifyPropertyChanged("next", oldStr, toStr(next));
        }
    }
}
