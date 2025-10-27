#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/config.h"

namespace melvin {

// Calculates score = frequency / (1 + age * decay_rate) and deletes low-score nodes/edges
class PruningEngine {
public:
    PruningEngine(AtomicGraph* graph);
    
    // Prune low-score nodes and edges
    size_t prune_nodes(float threshold = PRUNING_THRESHOLD);
    size_t prune_edges(float threshold = PRUNING_THRESHOLD);
    
    // Calculate score for a node
    float calculate_node_score(NodeID node) const;
    
    // Calculate score for an edge
    float calculate_edge_score(NodeID source, NodeID target) const;
    
private:
    AtomicGraph* graph_;
    
    Time get_node_age(NodeID node) const;
    uint32_t get_node_frequency(NodeID node) const;
};

} // namespace melvin

