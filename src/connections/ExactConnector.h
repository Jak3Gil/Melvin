#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/config.h"
#include <vector>

namespace melvin {

// Connects new nodes to X nodes before and after them (temporal locality)
class ExactConnector {
public:
    ExactConnector(AtomicGraph* graph);
    
    // Connect a new node to its temporal neighbors
    void connect_temporal_neighbors(NodeID new_node_id, size_t radius = EXACT_CONNECTION_RADIUS);
    
    // Get nodes within radius (before and after)
    std::vector<NodeID> get_temporal_neighbors(NodeID center, size_t radius) const;
    
private:
    AtomicGraph* graph_;
    
    NodeID find_node_before(NodeID center) const;
    NodeID find_node_after(NodeID center) const;
};

} // namespace melvin

