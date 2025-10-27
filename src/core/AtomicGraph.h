#pragma once

#include "../include/melvin/types.h"
#include "Node.h"
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <memory>

namespace melvin {

class AtomicGraph {
public:
    AtomicGraph();
    ~AtomicGraph();
    
    // Node operations (thread-safe)
    bool add_node(std::unique_ptr<Node> node);
    Node* get_node(NodeID id);
    bool remove_node(NodeID id);
    
    // Edge operations (thread-safe)
    bool add_edge(NodeID source, NodeID target, EdgeWeight weight);
    bool remove_edge(NodeID source, NodeID target);
    EdgeWeight get_edge_weight(NodeID source, NodeID target) const;
    bool has_edge(NodeID source, NodeID target) const;
    
    // Graph queries
    std::vector<NodeID> get_neighbors(NodeID node) const;
    std::vector<Edge> get_all_edges() const;
    std::vector<NodeID> get_all_nodes() const;
    
    // Check if payload already exists (for deduplication) - O(1) hash lookup
    NodeID find_node_with_payload(const void* payload, size_t payload_size) const;
    
    // Hash-based fast lookup
    uint64_t hash_payload(const void* payload, size_t size) const;
    
    // Edge weight updates (atomic)
    bool increment_edge_weight(NodeID source, NodeID target, EdgeWeight delta = 1);
    bool average_edge_weight(NodeID source, NodeID target, EdgeWeight other_weight);
    
    // Reconnect edges (used in leap node consolidation)
    void redirect_edge(NodeID old_target, NodeID new_target);
    
    // Statistics
    size_t node_count() const;
    size_t edge_count() const;
    
    // Reset
    void clear();
    
private:
    mutable std::shared_mutex nodes_mutex_;
    mutable std::shared_mutex edges_mutex_;
    
    std::unordered_map<NodeID, std::unique_ptr<Node>> nodes_;
    std::unordered_map<NodeID, std::unordered_map<NodeID, EdgeWeight>> edges_;
    
    // Hash map for fast payload deduplication (hash -> node_id)
    mutable std::shared_mutex payload_hash_mutex_;
    std::unordered_map<uint64_t, NodeID> payload_hash_to_node_;
    
    // Helper to get edge key
    static uint64_t edge_key(NodeID source, NodeID target) {
        return (static_cast<uint64_t>(source) << 32) | target;
    }
};

} // namespace melvin

