#include "CoherenceCalculator.h"
#include "../include/melvin/types.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>

namespace melvin {

CoherenceCalculator::CoherenceCalculator(AtomicGraph* graph) : graph_(graph) {
}

float CoherenceCalculator::calculate_coherence(const std::vector<NodeID>& active_nodes) const {
    float internal = avg_internal_weight(active_nodes);
    float external = avg_external_weight(active_nodes);
    
    if (external == 0.0f) {
        return internal > 0.0f ? 1.0f : 0.0f;
    }
    
    return internal / external;
}

float CoherenceCalculator::avg_internal_weight(const std::vector<NodeID>& active_nodes) const {
    if (active_nodes.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t count = 0;
    
    std::unordered_set<NodeID> active_set(active_nodes.begin(), active_nodes.end());
    
    for (NodeID node : active_nodes) {
        std::vector<NodeID> neighbors = graph_->get_neighbors(node);
        for (NodeID neighbor : neighbors) {
            if (active_set.find(neighbor) != active_set.end()) {
                EdgeWeight weight = graph_->get_edge_weight(node, neighbor);
                sum += static_cast<float>(weight) / 65535.0f;
                count++;
            }
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

float CoherenceCalculator::avg_external_weight(const std::vector<NodeID>& active_nodes) const {
    if (active_nodes.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    size_t count = 0;
    
    std::unordered_set<NodeID> active_set(active_nodes.begin(), active_nodes.end());
    
    for (NodeID node : active_nodes) {
        std::vector<NodeID> neighbors = graph_->get_neighbors(node);
        for (NodeID neighbor : neighbors) {
            if (active_set.find(neighbor) == active_set.end()) {
                EdgeWeight weight = graph_->get_edge_weight(node, neighbor);
                sum += static_cast<float>(weight) / 65535.0f;
                count++;
            }
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

float CoherenceCalculator::get_external_relevance(NodeID node, const std::vector<NodeID>& active_nodes) const {
    std::unordered_set<NodeID> active_set(active_nodes.begin(), active_nodes.end());
    std::vector<NodeID> neighbors = graph_->get_neighbors(node);
    
    float avg_weight = 0.0f;
    size_t count = 0;
    
    for (NodeID neighbor : neighbors) {
        if (active_set.find(neighbor) != active_set.end()) {
            EdgeWeight weight = graph_->get_edge_weight(node, neighbor);
            avg_weight += static_cast<float>(weight) / 65535.0f;
            count++;
        }
    }
    
    return count > 0 ? avg_weight / count : 0.0f;
}

} // namespace melvin

