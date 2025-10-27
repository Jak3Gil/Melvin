#include "LeapConnections.h"
#include "../include/melvin/types.h"
#include "../include/melvin/config.h"
#include <algorithm>
#include <set>
#include <iterator>

namespace melvin {

LeapConnections::LeapConnections(AtomicGraph* graph) : graph_(graph) {
}

std::vector<std::pair<NodeID, NodeID>> LeapConnections::find_candidates(float threshold) const {
    std::vector<std::pair<NodeID, NodeID>> candidates;
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    
    // Check all pairs
    for (size_t i = 0; i < all_nodes.size(); ++i) {
        for (size_t j = i + 1; j < all_nodes.size(); ++j) {
            NodeID n1 = all_nodes[i];
            NodeID n2 = all_nodes[j];
            
            NodeID common_target = 0;
            if (connect_to_same_target(n1, n2, common_target)) {
                float avg_weight = avg_weight_to_target(n1, n2, common_target);
                if (avg_weight > threshold) {
                    candidates.push_back({n1, n2});
                }
            }
        }
    }
    
    return candidates;
}

bool LeapConnections::create_leap_connection(NodeID n1, NodeID n2, float threshold) {
    NodeID common_target = 0;
    if (!connect_to_same_target(n1, n2, common_target)) {
        return false;
    }
    
    float avg_weight = avg_weight_to_target(n1, n2, common_target);
    if (avg_weight <= threshold) {
        return false;
    }
    
    // Create bidirectional connection with averaged weight
    EdgeWeight weight = static_cast<EdgeWeight>(avg_weight * 65535.0f);
    graph_->add_edge(n1, n2, weight);
    graph_->add_edge(n2, n1, weight);
    
    return true;
}

bool LeapConnections::connect_to_same_target(NodeID n1, NodeID n2, NodeID& common_target) const {
    std::vector<NodeID> neighbors1 = graph_->get_neighbors(n1);
    std::vector<NodeID> neighbors2 = graph_->get_neighbors(n2);
    
    // Sort for efficient intersection
    std::sort(neighbors1.begin(), neighbors1.end());
    std::sort(neighbors2.begin(), neighbors2.end());
    
    // Find common neighbor
    std::vector<NodeID> common;
    std::set_intersection(neighbors1.begin(), neighbors1.end(),
                          neighbors2.begin(), neighbors2.end(),
                          std::back_inserter(common));
    
    if (common.empty()) {
        return false;
    }
    
    common_target = common[0]; // Use first common target
    return true;
}

float LeapConnections::avg_weight_to_target(NodeID n1, NodeID n2, NodeID target) const {
    EdgeWeight w1 = graph_->get_edge_weight(n1, target);
    EdgeWeight w2 = graph_->get_edge_weight(n2, target);
    
    // Normalize to [0, 1] and average
    float avg = ((w1 / 65535.0f) + (w2 / 65535.0f)) / 2.0f;
    return avg;
}

} // namespace melvin

