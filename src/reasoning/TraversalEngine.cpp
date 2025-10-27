#include "TraversalEngine.h"
#include "../include/melvin/types.h"
#include <unordered_set>
#include <algorithm>

namespace melvin {

TraversalEngine::TraversalEngine(AtomicGraph* graph, ActivationField* field, CoherenceCalculator* coherence)
    : graph_(graph), field_(field), coherence_(coherence) {
}

NodeID TraversalEngine::select_next_node(const std::vector<NodeID>& active_nodes) {
    if (active_nodes.empty()) {
        // No active nodes, return first node in graph
        std::vector<NodeID> all_nodes = graph_->get_all_nodes();
        if (!all_nodes.empty()) {
            return all_nodes[0];
        }
        return 0;
    }
    
    // Find candidate nodes (neighbors of active nodes that aren't already active)
    std::unordered_set<NodeID> active_set(active_nodes.begin(), active_nodes.end());
    std::unordered_set<NodeID> candidates;
    
    for (NodeID active : active_nodes) {
        std::vector<NodeID> neighbors = graph_->get_neighbors(active);
        for (NodeID neighbor : neighbors) {
            if (active_set.find(neighbor) == active_set.end()) {
                candidates.insert(neighbor);
            }
        }
    }
    
    // Select candidate with highest score
    NodeID best_node = 0;
    float best_score = -1.0f;
    
    for (NodeID candidate : candidates) {
        float score = calculate_node_score(candidate, active_nodes);
        if (score > best_score) {
            best_score = score;
            best_node = candidate;
        }
    }
    
    return best_node;
}

float TraversalEngine::calculate_node_score(NodeID candidate, const std::vector<NodeID>& active_nodes) {
    // Combine coherence, edge strength, and external relevance
    float coherence = coherence_->calculate_coherence(active_nodes);
    float external_relevance = coherence_->get_external_relevance(candidate, active_nodes);
    
    // Calculate average edge strength to active nodes
    float avg_edge_strength = 0.0f;
    size_t count = 0;
    
    for (NodeID active : active_nodes) {
        EdgeWeight weight = graph_->get_edge_weight(active, candidate);
        avg_edge_strength += static_cast<float>(weight) / 65535.0f;
        count++;
    }
    
    if (count > 0) {
        avg_edge_strength /= count;
    }
    
    // Weighted combination
    float score = coherence * 0.4f + avg_edge_strength * 0.3f + external_relevance * 0.3f;
    
    return score;
}

bool TraversalEngine::should_continue_reasoning(const std::vector<NodeID>& active_nodes) {
    if (active_nodes.empty()) {
        return false;
    }
    
    float coherence = coherence_->calculate_coherence(active_nodes);
    bool is_stable = field_->is_stable();
    
    return coherence >= MIN_COHERENCE_RATIO && !is_stable;
}

std::vector<NodeID> TraversalEngine::reason(const std::vector<NodeID>& initial_nodes, size_t max_iterations) {
    std::vector<NodeID> active = initial_nodes;
    
    // Initialize energies
    for (NodeID node : active) {
        field_->set_energy(node, 1.0f);
    }
    
    for (size_t i = 0; i < max_iterations; ++i) {
        // Update energies
        field_->update_energies();
        
        // Excite neighbors
        field_->excite_neighbors(active);
        
        // Get new active nodes
        active = field_->get_active_nodes();
        
        // Check stability
        if (field_->is_stable() || !should_continue_reasoning(active)) {
            break;
        }
    }
    
    return active;
}

} // namespace melvin

