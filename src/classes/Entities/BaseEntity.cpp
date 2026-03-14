/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */

/**
 * @file BaseEntity.cpp
 * @brief Implementation of the BaseEntity class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "BaseEntity.h"

namespace ADS::Entities {
    BaseEntity::BaseEntity(const std::string& id, const std::string& name)
        : m_id(id), m_name(name) {
    }

    void BaseEntity::notifyPropertyChanged(
        const std::string& propertyId,
        const Inspector::PropertyValue& oldValue,
        const Inspector::PropertyValue& newValue
    ) {
        Inspector::PropertyChangedEvent event(propertyId, oldValue, newValue, this);
        m_eventDispatcher.dispatch(event);
    }

    std::string BaseEntity::getDisplayName() const {
        return m_name;
    }

    Inspector::PropertyEventDispatcher& BaseEntity::getEventDispatcher() {
        return m_eventDispatcher;
    }

    const std::string& BaseEntity::getId() const {
        return m_id;
    }

    void BaseEntity::setName(const std::string& name) {
        if (m_name != name) {
            std::string oldName = m_name;
            m_name = name;
            notifyPropertyChanged("name", oldName, m_name);
        }
    }
}
