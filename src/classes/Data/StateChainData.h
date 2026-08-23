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

#ifndef ADS_DATA_STATE_CHAIN_DATA_H
#define ADS_DATA_STATE_CHAIN_DATA_H

/**
 * @file StateChainData.h
 * @brief Pure data class for a named state chain — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */

#include <optional>

#include "BaseData.h"
#include "Types/Id.h"

namespace ADS::Data {

    /**
     * @brief Pure data class for a named chain of state-catalog entries.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * A thin, named/id'd handle onto a chain of Data::StateData entries
     * linked via their own next() fields — the chain itself doesn't own or
     * order its states, it just names the entry point. Entities (Scene
     * today) reference a chain by id rather than a specific state directly:
     * selecting a chain resolves to its head state, and the chain's own
     * name is what stays shown as the selected value (see
     * Entities::StateChain::buildChainOptionLabels()). Only standard C++
     * types are used — no ImGui or UI framework types appear here. This
     * class is owned by Core::Project; Entities::StateChain reads/writes
     * through a non-owning pointer to this object.
     */
    class StateChainData : public BaseData<ADS::Types::ChainTag> {
    public:
        /**
         * @brief Get the head state of this chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::optional<ADS::Types::StateId>& Head state id,
         *         or std::nullopt if this chain has no states yet
         */
        [[nodiscard]] const std::optional<ADS::Types::StateId>& getHead() const { return m_head; }

        /**
         * @brief Set the head state of this chain
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param head The new head state id, or std::nullopt to clear it
         */
        void setHead(const std::optional<ADS::Types::StateId>& head) { m_head = head; }

    private:
        std::optional<ADS::Types::StateId> m_head; ///< Head state of this chain, or std::nullopt if empty
    };

} // namespace ADS::Data

#endif // ADS_DATA_STATE_CHAIN_DATA_H
