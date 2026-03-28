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

#ifndef ADS_PROPERTY_EVENT_H
#define ADS_PROPERTY_EVENT_H

#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "PropertyValue.h"

namespace ADS::Inspector {
    // Forward declaration
    class IInspectable;

    /**
     * @brief Event data for property changes
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Contains all information about a property change event,
     * including the property identifier, old and new values,
     * and the source object that generated the event.
     */
    struct PropertyChangedEvent {
        std::string propertyId;     ///< The ID of the changed property
        PropertyValue oldValue;     ///< The previous value
        PropertyValue newValue;     ///< The new value
        IInspectable* source;       ///< The object that generated the event

        PropertyChangedEvent() : source(nullptr) {}

        PropertyChangedEvent(
            std::string id,
            PropertyValue oldVal,
            PropertyValue newVal,
            IInspectable* src
        ) : propertyId(std::move(id)), oldValue(std::move(oldVal)), newValue(std::move(newVal)), source(src) {}
    };

    /**
     * @brief Callback type for property change subscribers
     */
    using PropertyChangedCallback = std::function<void(const PropertyChangedEvent&)>;

    /**
     * @brief Handle type for managing subscriptions
     */
    using SubscriptionHandle = size_t;

    /**
     * @brief Thread-safe event dispatcher for property changes
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Manages subscriptions and dispatches property change events
     * to all registered listeners. Each IInspectable object owns
     * its own dispatcher instance.
     *
     * Thread-safe: All operations are protected by a mutex.
     */
    class PropertyEventDispatcher {
    private:
        std::unordered_map<SubscriptionHandle, PropertyChangedCallback> m_subscribers;
        SubscriptionHandle m_nextHandle;
        mutable std::mutex m_mutex;

    public:
        /**
         * @brief Construct a new PropertyEventDispatcher
         */
        PropertyEventDispatcher();

        /**
         * @brief Destroy the PropertyEventDispatcher
         *
         * Clears all subscriptions.
         */
        ~PropertyEventDispatcher();

        // Non-copyable, non-movable
        PropertyEventDispatcher(const PropertyEventDispatcher&) = delete;
        PropertyEventDispatcher& operator=(const PropertyEventDispatcher&) = delete;
        PropertyEventDispatcher(PropertyEventDispatcher&&) = delete;
        PropertyEventDispatcher& operator=(PropertyEventDispatcher&&) = delete;

        /**
         * @brief Subscribe to property change events - lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param callback Function to call when a property changes
         * @return SubscriptionHandle Handle for unsubscribing
         */
        SubscriptionHandle subscribe(const PropertyChangedCallback& callback);

        /**
         * @brief Subscribe to property change events - rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param callback Function to call when a property changes
         * @return SubscriptionHandle Handle for unsubscribing
         */
        SubscriptionHandle subscribe(PropertyChangedCallback&& callback);

        /**
         * @brief Unsubscribe from property change events
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param handle The subscription handle returned by subscribe()
         */
        void unsubscribe(SubscriptionHandle handle);

        /**
         * @brief Dispatch a property change event to all subscribers
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param event The event to dispatch
         */
        void dispatch(const PropertyChangedEvent& event);

        /**
         * @brief Get the number of active subscribers
         *
         * @return size_t Number of subscribers
         */
        size_t getSubscriberCount() const;

        /**
         * @brief Remove all subscribers
         */
        void clear();
    };
}

#endif //ADS_PROPERTY_EVENT_H