// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OPENCBDC_TX_SRC_SENTINEL_INTERFACE_H_
#define OPENCBDC_TX_SRC_SENTINEL_INTERFACE_H_

#include "common/hash.hpp"
#include "network/connection_manager.hpp"
#include "transaction/validation.hpp"

#include <optional>
#include <string>

namespace cbdc::sentinel {
    /// Status of the transaction following sentinel processing.
    enum class tx_status {
        /// Statically valid, and the sentinel has submitted
        /// the transaction to the network for processing.
        pending,
        /// Statically invalid. Must be fixed and resubmitted.
        static_invalid,
        /// Statically valid, but rejected by the shards for trying to spend
        /// inputs either that do not exist or that a previous transaction
        /// already spent.
        state_invalid,
        /// Executed to completion. Included in a block generated by the
        /// atomizer cluster or completed by a distributed transaction batch
        /// coordinated between locking shards.
        confirmed
    };

    /// Sentinel-specific representation of shard network information.
    struct shard_info {
        /// UHS ID range.
        config::shard_range_t m_range;
        /// Network ID of the peer corresponding to this shard generated by a
        /// call to cbdc::network::network::connect.
        cbdc::network::peer_id_t m_peer_id;
    };

    /// Return a human-readable string describing a \ref tx_status.
    /// \param status tx_status to print.
    /// \return human-readable string.
    auto to_string(tx_status status) -> std::string;

    /// Sentinel request message.
    using request = transaction::full_tx;

    /// Sentinel response message.
    struct response {
        /// Transaction execution status.
        cbdc::sentinel::tx_status m_tx_status{};
        /// Transaction validation error if static validation failed.
        std::optional<transaction::validation::tx_error> m_tx_error;

        auto operator==(const response& rhs) const -> bool;
    };

    /// Interface for a sentinel.
    class interface {
      public:
        virtual ~interface() = default;

        interface() = default;
        interface(const interface&) = delete;
        auto operator=(const interface&) -> interface& = delete;
        interface(interface&&) = delete;
        auto operator=(interface&&) -> interface& = delete;

        /// Validate transaction on the sentinel, and forward it to the
        /// coordinator or shards depending on the implementation. Return the
        /// execution result.
        /// \param tx transaction to execute.
        /// \return the response from the sentinel or std::nullopt if
        ///         processing failed.
        virtual auto execute_transaction(transaction::full_tx tx)
            -> std::optional<cbdc::sentinel::response> = 0;
    };
}

#endif