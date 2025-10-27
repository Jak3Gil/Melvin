#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/config.h"
#include <vector>

namespace melvin {

// Creates shortcut connections when 2 nodes connect to same target with avg weight > Lw
class LeapConnections {
public:
    LeapConnections(AtomicGraph* graph);
    
    // Find candidates for leap connections
    std::vector<std::pair<NodeID, NodeID>> find_candidates(float threshold = DEFAULT_LW) const;
    
    // Create leap connection between two nodes
    bool create_leap_connection(NodeID n1, NodeID n2, float threshold = DEFAULT_LW);
    
    // Check if two nodes connect to the same target
    bool connect_to_same_target(NodeID n1, NodeID n2, NodeID& common_target) const;
    
    // Calculate average weight of connections to common target
    float avg_weight_to_target(NodeID n1, NodeID n2, NodeID target) const;
    
private:
    AtomicGraph* graph_;
};

} // namespace melvin

