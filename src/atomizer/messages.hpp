// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OPENCBDC_TX_SRC_ATOMIZER_MESSAGES_H_
#define OPENCBDC_TX_SRC_ATOMIZER_MESSAGES_H_

#include "block.hpp"
#include "transaction/transaction.hpp"
#include "watchtower/tx_error_messages.hpp"

namespace cbdc::atomizer {
    /// \brief Transaction notification message.
    ///
    /// Sent from shards to the atomizer. Notifies the atomizer that a shard
    /// has received a transaction from a sentinel. The shard attaches an
    /// attestation for each transaction input that is covered by the shard's
    /// UHS subset, and currently unspent in the UHS. The shard also attaches
    /// the block height at which the attestations are valid.
    struct tx_notify_request {
        auto operator==(const tx_notify_request& rhs) const -> bool;

        /// Compact transaction associated with the notification.
        transaction::compact_tx m_tx;
        /// Set of input indexes the shard is attesting are unspent at the
        /// given block height.
        std::unordered_set<uint64_t> m_attestations;
        /// Block height at which the given input attestations are valid.
        uint64_t m_block_height{};
    };

    /// \brief Transaction notification message with a full set of input
    ///        attestations.
    ///
    /// The atomizer manager ( \ref atomizer_raft ) sends this message to the
    /// atomizer state machine ( \ref state_machine ) once it has received a
    /// full set of input attestations for a given compact transaction. The
    /// atomizer manager attaches the block height of the oldest attesation
    /// used to build the full set. The structure is used as an optimization
    /// to remove the need to replicate individual transaction notifications
    /// in the atomizer cluster.
    struct aggregate_tx_notification {
        auto operator==(const aggregate_tx_notification& rhs) const -> bool;

        /// Compact transaction associated with the notification.
        transaction::compact_tx m_tx;
        /// Block height of the oldest input attestation used to build this
        /// aggregate notification.
        uint64_t m_oldest_attestation{};
    };

    /// \brief Batch of aggregate transaction notifications.
    ///
    /// Atomizer state machine ( \ref state_machine ) message containing a
    /// batch of \ref aggregate_tx_notification.
    struct aggregate_tx_notify_request {
        auto operator==(const aggregate_tx_notify_request& rhs) const -> bool;

        /// Batch of aggregate transaction notifications.
        std::vector<aggregate_tx_notification> m_agg_txs;
    };

    /// Placeholder struct for a make block state machine request.
    struct make_block_request {};

    /// Prune blocks request for RPC and state machine.
    struct prune_request {
        /// Block height below which to prune cached blocks.
        uint64_t m_block_height{};
    };

    /// Retrieve cached block request.
    struct get_block_request {
        /// Block height to retrieve.
        uint64_t m_block_height{};
    };

    /// List of watchtower errors returned by the atomizer state machine.
    using errors = std::vector<watchtower::tx_error>;

    /// Response from atomizer state machine to a make block request.
    struct make_block_response {
        /// Block generated by request.
        block m_blk;
        /// Watchtower errors resulting from block creation.
        errors m_errs;
    };

    /// Atomizer state machine response from get block request.
    struct get_block_response {
        /// Block returned by request.
        block m_blk;
    };

    /// Atomizer RPC request.
    using request
        = std::variant<tx_notify_request, prune_request, get_block_request>;
}

#endif
