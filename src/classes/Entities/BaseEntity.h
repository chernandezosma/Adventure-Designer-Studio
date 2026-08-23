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

#ifndef ADS_BASE_ENTITY_H
#define ADS_BASE_ENTITY_H

#include <string>
#include "Data/IIdentifiable.h"
#include "Inspector/IInspectable.h"
#include "Inspector/PropertyEvent.h"

namespace ADS::Core {
    class Project;
}

namespace ADS::Entities {
    /**
     * @brief Base class for all game entities (inspector adapter layer)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Provides common implementation for IInspectable interface,
     * including event dispatcher management and property change
     * notification helpers. Holds a non-owning pointer to a
     * Data::IIdentifiable, the tag-erased interface implemented by every
     * BaseData<Tag> instantiation, which is the authoritative storage for
     * id and name. The DataObject is owned by Core::Project. Concrete
     * entities (Scene, Item, Character) additionally expose their own
     * typed id accessor (e.g. Scene::getSceneId()) for callers that need
     * the underlying ADS::Types::Id<Tag> rather than its display string.
     */
    class BaseEntity : public Inspector::IInspectable {
    protected:
        Data::IIdentifiable* m_baseData; ///< Non-owning pointer to the backing DataObject

        /// Non-owning pointer to the owning Project, set via setProject()
        /// right after construction. Used by concrete entities to resolve
        /// cross-entity option lists (e.g. "every scene in the project")
        /// for Select-type properties. May be nullptr for entities not
        /// constructed through Core::Project (e.g. isolated unit tests).
        Core::Project* m_project = nullptr;

        /// Event dispatcher for property changes
        Inspector::PropertyEventDispatcher m_eventDispatcher;

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
        void notifyPropertyChanged(
            const std::string& propertyId,
            const Inspector::PropertyValue& oldValue,
            const Inspector::PropertyValue& newValue
        );

        /**
         * @brief Translate a translation key via the process-wide active i18n instance
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Shared by every concrete entity's getPropertyDescriptors() to
         * translate display names, category names, descriptions, and
         * option labels. Falls back to returning @p key unchanged when no
         * i18n instance is active (e.g. isolated unit tests that never
         * construct ADS::Core::App) — never throws or crashes.
         *
         * @param key Dotted translation key (e.g. "SCENE.PROP_NAME")
         * @return std::string Translated text, or @p key if untranslated
         */
        static std::string translate(const std::string& key);

    public:
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
        explicit BaseEntity(Data::IIdentifiable* data);

        /**
         * @brief Virtual destructor
         */
        ~BaseEntity() override = default;

        // IInspectable interface

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
        std::string getDisplayName() const override;

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
        Inspector::PropertyEventDispatcher& getEventDispatcher() override;

        /**
         * @brief Get the unique identifier
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Tag-erased, generic form of the entity id, for code (e.g. UI
         * panels) that treats Scene/Item/Character ids uniformly as
         * strings. See the concrete entity's typed id accessor (e.g.
         * Scene::getSceneId()) when the underlying ADS::Types::Id<Tag> is
         * needed instead.
         *
         * @return std::string Entity ID read from the backing DataObject
         */
        std::string getId() const;

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
        void setName(const std::string& name);

        /**
         * @brief Set the owning Project back-pointer
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Called by Core::Project right after constructing this entity.
         *
         * @param project Non-owning pointer to the owning Project
         */
        void setProject(Core::Project* project);

        /**
         * @brief Get the owning Project back-pointer
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return Core::Project* Non-owning pointer, or nullptr if unset
         */
        [[nodiscard]] Core::Project* getProject() const;

    protected:
        /**
         * @brief Set a DataObject field and fire a property-changed event
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Reads the current value via @p getter, compares it to @p newValue,
         * and only writes and notifies if the value actually changed. Handles
         * all scalar and string properties (bool, int, std::string). Color
         * properties require manual handling due to the ADS::Types::Color ↔
         * ImVec4 conversion needed for notifyPropertyChanged.
         *
         * @tparam T        Property value type (deduced from newValue)
         * @tparam Getter   Callable returning T — reads from the DataObject
         * @tparam Setter   Callable taking const T& — writes to the DataObject
         * @param propertyId String key used for the change event
         * @param getter     Lambda that returns the current value
         * @param setter     Lambda that applies the new value
         * @param newValue   The value to set
         */
        template<typename T, typename Getter, typename Setter>
        void setAndNotify(const std::string& propertyId,
                          Getter getter, Setter setter, const T& newValue)
        {
            T current = getter();
            if (current != newValue) {
                setter(newValue);
                notifyPropertyChanged(propertyId, current, newValue);
            }
        }
    };
}

#endif //ADS_BASE_ENTITY_H
