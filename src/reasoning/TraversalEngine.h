#pragma once

#include "../core/AtomicGraph.h"
#include "ActivationField.h"
#include "CoherenceCalculator.h"
#include "../include/melvin/config.h"
#include <vector>

namespace melvin {

// Decides next activation by combining coherence, edge strength, and external relevance
class TraversalEngine {
public:
    TraversalEngine(AtomicGraph* graph, ActivationField* field, CoherenceCalculator* coherence);
    
    // Get next node to activate based on current active nodes
    NodeID select_next_node(const std::vector<NodeID>& active_nodes);
    
    // Decide if we should continue reasoning
    bool should_continue_reasoning(const std::vector<NodeID>& active_nodes);
    
    // Run reasoning cycle
    std::vector<NodeID> reason(const std::vector<NodeID>& initial_nodes, 
                               size_t max_iterations = MAX_REASONING_ITERATIONS);
    
private:
    AtomicGraph* graph_;
    ActivationField* field_;
    CoherenceCalculator* coherence_;
    
    // Calculate score for potential next node
    float calculate_node_score(NodeID candidate, const std::vector<NodeID>& active_nodes);
};

} // namespace melvin

