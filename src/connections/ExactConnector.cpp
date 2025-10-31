#include "ExactConnector.h"
#include "../include/melvin/types.h"
#include <algorithm>

namespace melvin {

ExactConnector::ExactConnector(AtomicGraph* graph) : graph_(graph) {
}

void ExactConnector::connect_temporal_neighbors(NodeID new_node_id, size_t radius) {
    std::vector<NodeID> neighbors = get_temporal_neighbors(new_node_id, radius);
    
    for (NodeID neighbor_id : neighbors) {
        // Connect bidirectionally with initial weight
        graph_->add_edge(new_node_id, neighbor_id, INITIAL_EDGE_WEIGHT);
        graph_->add_edge(neighbor_id, new_node_id, INITIAL_EDGE_WEIGHT);
    }
}

std::vector<NodeID> ExactConnector::get_temporal_neighbors(NodeID center, size_t radius) const {
    std::vector<NodeID> neighbors;
    neighbors.reserve(radius * 2);
    
    // Get all nodes
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    
    // Sort by ID (temporal order)
    std::sort(all_nodes.begin(), all_nodes.end());
    
    // Find center node
    auto center_it = std::lower_bound(all_nodes.begin(), all_nodes.end(), center);
    if (center_it == all_nodes.end() || *center_it != center) {
        return neighbors; // Node not found
    }
    
    size_t center_idx = center_it - all_nodes.begin();
    
    // Add nodes before
    for (size_t i = center_idx; i > 0 && (center_idx - i) < radius; --i) {
        neighbors.push_back(all_nodes[i - 1]);
    }
    
    // Add nodes after
    for (size_t i = center_idx + 1; i < all_nodes.size() && (i - center_idx - 1) < radius; ++i) {
        neighbors.push_back(all_nodes[i]);
    }
    
    return neighbors;
}

NodeID ExactConnector::find_node_before(NodeID center) const {
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    std::sort(all_nodes.begin(), all_nodes.end());
    
    auto it = std::lower_bound(all_nodes.begin(), all_nodes.end(), center);
    if (it != all_nodes.end() && *it == center && it != all_nodes.begin()) {
        return *(it - 1);
    }
    return 0;
}

NodeID ExactConnector::find_node_after(NodeID center) const {
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    std::sort(all_nodes.begin(), all_nodes.end());
    
    auto it = std::upper_bound(all_nodes.begin(), all_nodes.end(), center);
    if (it != all_nodes.end()) {
        return *it;
    }
    return 0;
}

} // namespace melvin

