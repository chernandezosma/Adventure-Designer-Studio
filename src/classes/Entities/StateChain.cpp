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
 * @file StateChain.cpp
 * @brief Implementation of the StateChain entity (inspector adapter)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include "StateChain.h"
#include "Core/Project.h"

namespace {
    /// Index of the state with the given id within the free-states list
    /// (project->getFreeStates(std::nullopt, id) — id itself is kept
    /// visible even though it's "used" by this very field), or -1 if
    /// absent/unresolvable. Must iterate in the same order as
    /// StateChain::buildStateOptionLabels() so indices line up.
    int findStateOptionIndex(ADS::Core::Project* project, const std::optional<ADS::Types::StateId>& id) {
        if (!project || !id.has_value()) return -1;
        auto states = project->getFreeStates(std::nullopt, id);
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->getStateId() == *id) return static_cast<int>(i);
        }
        return -1;
    }
}

namespace ADS::Entities {
    /**
     * @brief Construct a new StateChain backed by the given StateChainData
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param data Non-owning pointer to the StateChainData struct. Must not
     *             be null and must outlive this entity.
     */
    StateChain::StateChain(Data::StateChainData *data) :
        BaseEntity(data), m_data(data)
    {
    }

    /**
     * @brief Get the typed chain identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return ADS::Types::ChainId The chain's typed identifier
     */
    ADS::Types::ChainId StateChain::getChainId() const
    {
        return m_data->getId();
    }

    /**
     * @brief Get the type name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::string Translated "Chain" type name
     */
    std::string StateChain::getTypeName() const
    {
        return translate("CHAIN.TYPE_NAME");
    }

    /**
     * @brief Get the list of property descriptors for this chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return std::vector<Inspector::PropertyDescriptor> Property descriptors
     */
    std::vector<Inspector::PropertyDescriptor> StateChain::getPropertyDescriptors() const
    {
        using namespace Inspector;

        return {
            // Info category (read-only)
            PropertyDescriptor("id", translate("CHAIN.PROP_ID"), PropertyType::String)
            .setCategory(translate("CATEGORY.INFO"))
            .setDescription(translate("CHAIN.DESC_ID"))
            .setReadOnly(),

            // General category
            PropertyDescriptor("name", translate("CHAIN.PROP_NAME"), PropertyType::String)
            .setCategory(translate("CATEGORY.GENERAL"))
            .setDescription(translate("CHAIN.DESC_NAME"))
            .setConstraints(PropertyConstraints::string(128)),

            PropertyDescriptor("head", translate("CHAIN.PROP_HEAD"), PropertyType::Select)
            .setCategory(translate("CATEGORY.GENERAL"))
            .setDescription(translate("CHAIN.DESC_HEAD"))
            .setOptionsProvider([this] { return buildStateOptionLabels(); })
            .setEmptyOptionsText("INSPECTOR.NO_STATES")
            .setAllowCreateNew(),
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
    Inspector::PropertyValue StateChain::getPropertyValue(const std::string &propertyId) const
    {
        if (propertyId == "id")
            return std::to_string(m_data->getId().value);
        if (propertyId == "name")
            return m_data->getName();
        if (propertyId == "head") {
            std::vector<std::string> options = buildStateOptionLabels();
            std::vector<int> selected;
            int idx = findStateOptionIndex(getProject(), m_data->getHead());
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
    bool StateChain::setPropertyValue(const std::string &propertyId, const Inspector::PropertyValue &value)
    {
        if (propertyId == "name") {
            if (auto *str = std::get_if<std::string>(&value)) {
                setName(*str);
                return true;
            }
        } else if (propertyId == "head") {
            if (auto *sel = std::get_if<Inspector::SelectValue>(&value)) {
                std::optional<ADS::Types::StateId> newTarget;
                if (!sel->selectedIndices.empty() && getProject()) {
                    auto states = getProject()->getFreeStates(std::nullopt, m_data->getHead());
                    int idx = sel->selectedIndices.front();
                    if (idx >= 0 && idx < static_cast<int>(states.size())) {
                        newTarget = states[idx]->getStateId();
                    }
                }
                setHead(newTarget);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Build the option-label list for states available as this chain's head
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Only "free" states — any state already claimed as another state's
     * next or a chain's head are excluded, except whichever state this
     * chain's own head currently points to (kept visible so it doesn't
     * vanish from the dropdown). See Core::Project::getFreeStates().
     *
     * @return std::vector<std::string> One label per selectable state
     */
    std::vector<std::string> StateChain::buildStateOptionLabels() const
    {
        std::vector<std::string> labels;
        if (!getProject()) return labels;
        for (auto* state : getProject()->getFreeStates(std::nullopt, m_data->getHead())) {
            labels.push_back(state->getDisplayName());
        }
        return labels;
    }

    /**
     * @brief Get the head state of this chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return const std::optional<ADS::Types::StateId>& Head state id, or
     *         std::nullopt if this chain has no states yet
     */
    const std::optional<ADS::Types::StateId>& StateChain::getHead() const
    {
        return m_data->getHead();
    }

    /**
     * @brief Set the head state of this chain
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Fires a property-changed event if the value actually changed.
     *
     * @param head The new head state id, or std::nullopt to clear it
     */
    void StateChain::setHead(const std::optional<ADS::Types::StateId> &head)
    {
        if (m_data->getHead() != head) {
            auto toStr = [](const std::optional<ADS::Types::StateId>& id) {
                return id.has_value() ? std::to_string(id->value) : std::string();
            };
            std::string oldStr = toStr(m_data->getHead());
            m_data->setHead(head);
            notifyPropertyChanged("head", oldStr, toStr(head));
        }
    }
}
