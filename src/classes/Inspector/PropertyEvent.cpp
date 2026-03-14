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
 * @file PropertyEvent.cpp
 * @brief Implementation of the PropertyEventDispatcher class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "PropertyEvent.h"

namespace ADS::Inspector {
    /**
     * @brief Construct a new PropertyEventDispatcher
     */
    PropertyEventDispatcher::PropertyEventDispatcher()
        : m_nextHandle(1) {
    }

    /**
     * @brief Destroy the PropertyEventDispatcher
     *
     * Clears all subscriptions.
     */
    PropertyEventDispatcher::~PropertyEventDispatcher() {
        clear();
    }

    /**
     * @brief Subscribe to property change events - lvalue version
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param callback Function to call when a property changes
     * @return SubscriptionHandle Handle for unsubscribing
     */
    SubscriptionHandle PropertyEventDispatcher::subscribe(const PropertyChangedCallback& callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        SubscriptionHandle handle = m_nextHandle++;
        m_subscribers[handle] = callback;
        return handle;
    }

    /**
     * @brief Subscribe to property change events - rvalue version
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param callback Function to call when a property changes
     * @return SubscriptionHandle Handle for unsubscribing
     */
    SubscriptionHandle PropertyEventDispatcher::subscribe(PropertyChangedCallback&& callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        SubscriptionHandle handle = m_nextHandle++;
        m_subscribers[handle] = std::move(callback);
        return handle;
    }

    /**
     * @brief Unsubscribe from property change events
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param handle The subscription handle returned by subscribe()
     */
    void PropertyEventDispatcher::unsubscribe(SubscriptionHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscribers.erase(handle);
    }

    /**
     * @brief Dispatch a property change event to all subscribers
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param event The event to dispatch
     */
    void PropertyEventDispatcher::dispatch(const PropertyChangedEvent& event) {
        // Copy subscribers to avoid holding lock during callback execution
        std::unordered_map<SubscriptionHandle, PropertyChangedCallback> subscribersCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            subscribersCopy = m_subscribers;
        }

        // Dispatch to all subscribers
        for (const auto& [handle, callback] : subscribersCopy) {
            if (callback) {
                callback(event);
            }
        }
    }

    /**
     * @brief Get the number of active subscribers
     *
     * @return size_t Number of subscribers
     */
    size_t PropertyEventDispatcher::getSubscriberCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_subscribers.size();
    }

    /**
     * @brief Remove all subscribers
     */
    void PropertyEventDispatcher::clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_subscribers.clear();
    }
}