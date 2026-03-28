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
 * @file BaseEntity.cpp
 * @brief Implementation of the BaseEntity class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "BaseEntity.h"

namespace ADS::Entities {
    BaseEntity::BaseEntity(Data::BaseData* data)
        : m_baseData(data) {
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
        return m_baseData->name;
    }

    Inspector::PropertyEventDispatcher& BaseEntity::getEventDispatcher() {
        return m_eventDispatcher;
    }

    const std::string& BaseEntity::getId() const {
        return m_baseData->id;
    }

    void BaseEntity::setName(const std::string& name) {
        if (m_baseData->name != name) {
            std::string oldName = m_baseData->name;
            m_baseData->name = name;
            notifyPropertyChanged("name", oldName, m_baseData->name);
        }
    }
}
