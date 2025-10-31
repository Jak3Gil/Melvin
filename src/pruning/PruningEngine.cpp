#include "PruningEngine.h"
#include "../core/Node.h"
#include "../include/melvin/types.h"

namespace melvin {

PruningEngine::PruningEngine(AtomicGraph* graph) : graph_(graph) {
}

size_t PruningEngine::prune_nodes(float threshold) {
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    size_t pruned_count = 0;
    
    for (NodeID node_id : all_nodes) {
        float score = calculate_node_score(node_id);
        if (score < threshold) {
            graph_->remove_node(node_id);
            pruned_count++;
        }
    }
    
    return pruned_count;
}

size_t PruningEngine::prune_edges(float threshold) {
    std::vector<Edge> all_edges = graph_->get_all_edges();
    size_t pruned_count = 0;
    
    for (const Edge& edge : all_edges) {
        float score = calculate_edge_score(edge.source, edge.target);
        if (score < threshold) {
            graph_->remove_edge(edge.source, edge.target);
            pruned_count++;
        }
    }
    
    return pruned_count;
}

float PruningEngine::calculate_node_score(NodeID node) const {
    uint32_t frequency = get_node_frequency(node);
    Time age = get_node_age(node);
    
    float score = static_cast<float>(frequency) / (1.0f + static_cast<float>(age) * PRUNING_DECAY_RATE);
    return score;
}

float PruningEngine::calculate_edge_score(NodeID source, NodeID target) const {
    // Simplified: use edge weight as score
    EdgeWeight weight = graph_->get_edge_weight(source, target);
    return static_cast<float>(weight) / 65535.0f;
}

Time PruningEngine::get_node_age(NodeID node) const {
    Node* node_ptr = graph_->get_node(node);
    if (!node_ptr) {
        return 0;
    }
    
    Time current_time = 0; // TODO: get actual time
    Time first_seen = node_ptr->first_seen();
    
    return current_time > first_seen ? current_time - first_seen : 0;
}

uint32_t PruningEngine::get_node_frequency(NodeID node) const {
    Node* node_ptr = graph_->get_node(node);
    if (!node_ptr) {
        return 0;
    }
    
    return node_ptr->frequency();
}

} // namespace melvin

