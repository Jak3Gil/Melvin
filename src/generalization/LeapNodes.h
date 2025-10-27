#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/config.h"
#include <vector>

namespace melvin {

// Consolidates 3 fully-connected nodes when sum of weights > W (default 2.5)
class LeapNodes {
public:
    LeapNodes(AtomicGraph* graph);
    
    // Find candidates for leap node consolidation
    std::vector<std::vector<NodeID>> find_candidates(float threshold = DEFAULT_W) const;
    
    // Consolidate three nodes into one (uses oldest ID)
    bool consolidate_triple(NodeID n1, NodeID n2, NodeID n3, float threshold = DEFAULT_W);
    
    // Check if three nodes form a fully-connected triangle
    bool is_triangle(NodeID n1, NodeID n2, NodeID n3) const;
    
    // Calculate sum of edge weights in triangle
    float triangle_weight_sum(NodeID n1, NodeID n2, NodeID n3) const;
    
private:
    AtomicGraph* graph_;
    
    NodeID get_oldest_node(NodeID n1, NodeID n2, NodeID n3) const;
    void merge_connections(NodeID keep_id, NodeID delete_id1, NodeID delete_id2) const;
};

} // namespace melvin

