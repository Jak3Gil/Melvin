#pragma once

#include "../core/AtomicGraph.h"
#include <vector>

namespace melvin {

// Calculates coherence = avg(internal weights) / avg(external weights)
class CoherenceCalculator {
public:
    CoherenceCalculator(AtomicGraph* graph);
    
    // Calculate coherence for a set of active nodes
    float calculate_coherence(const std::vector<NodeID>& active_nodes) const;
    
    // Calculate average internal weight (active node connections to each other)
    float avg_internal_weight(const std::vector<NodeID>& active_nodes) const;
    
    // Calculate average external weight (active to inactive connections)
    float avg_external_weight(const std::vector<NodeID>& active_nodes) const;
    
    // Get external relevance for a node (how many connections to active nodes)
    float get_external_relevance(NodeID node, const std::vector<NodeID>& active_nodes) const;
    
private:
    AtomicGraph* graph_;
};

} // namespace melvin

