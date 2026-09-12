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
#include "i18n/i18n.h"

namespace ADS::Entities {
    /**
     * @brief Construct a new BaseEntity backed by the given DataObject
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param data Non-owning pointer to the BaseData<Tag> struct (as its
     *             tag-erased IIdentifiable interface). Must not be null
     *             and must outlive this entity (Core::Project guarantees this).
     */
    BaseEntity::BaseEntity(Data::IIdentifiable* data)
        : m_baseData(data) {
    }

    /**
     * @brief Notify subscribers of a property change
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Helper method for derived classes to fire property change events.
     *
     * @param propertyId The ID of the changed property
     * @param oldValue The previous value
     * @param newValue The new value
     */
    void BaseEntity::notifyPropertyChanged(
        const std::string& propertyId,
        const Inspector::PropertyValue& oldValue,
        const Inspector::PropertyValue& newValue
    ) {
        Inspector::PropertyChangedEvent event(propertyId, oldValue, newValue, this);
        m_eventDispatcher.dispatch(event);
    }

    /**
     * @brief Get the display name of this entity
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Reads the name from the backing DataObject.
     *
     * @return std::string Human-readable display name
     */
    std::string BaseEntity::getDisplayName() const {
        return m_baseData->getName();
    }

    /**
     * @brief Get the property event dispatcher
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Returns a reference to the dispatcher used to subscribe to and
     * fire property-changed events for this entity.
     *
     * @return Inspector::PropertyEventDispatcher& Reference to the event dispatcher
     */
    Inspector::PropertyEventDispatcher& BaseEntity::getEventDispatcher() {
        return m_eventDispatcher;
    }

    /**
     * @brief Translate a translation key via the process-wide active i18n instance
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param key Dotted translation key (e.g. "SCENE.PROP_NAME")
     * @return std::string Translated text, or @p key if untranslated
     */
    std::string BaseEntity::translate(const std::string& key) {
        if (auto* t = i18n::i18n::getActiveInstance()) {
            return t->_t(key);
        }
        return key;
    }

    /**
     * @brief Get the unique identifier
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @return std::string Entity ID read from the backing DataObject
     */
    std::string BaseEntity::getId() const {
        return m_baseData->getIdString();
    }

    /**
     * @brief Set the display name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Writes the new name into the backing DataObject and fires a
     * property-changed event if the value actually changed.
     *
     * @param name New display name
     */
    void BaseEntity::setName(const std::string& name) {
        if (m_baseData->getName() != name) {
            std::string oldName = m_baseData->getName();
            m_baseData->setName(name);
            notifyPropertyChanged("name", oldName, m_baseData->getName());
        }
    }

    /**
     * @brief Set the owning Project back-pointer
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @param project Non-owning pointer to the owning Project
     */
    void BaseEntity::setProject(Core::Project* project) {
        m_project = project;
    }

    /**
     * @brief Get the owning Project back-pointer
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * @return Core::Project* Non-owning pointer, or nullptr if unset
     */
    Core::Project* BaseEntity::getProject() const {
        return m_project;
    }
}
