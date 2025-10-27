#include "LeapNodes.h"
#include "../include/melvin/types.h"
#include <algorithm>
#include <numeric>

namespace melvin {

LeapNodes::LeapNodes(AtomicGraph* graph) : graph_(graph) {
}

std::vector<std::vector<NodeID>> LeapNodes::find_candidates(float threshold) const {
    std::vector<std::vector<NodeID>> candidates;
    std::vector<NodeID> all_nodes = graph_->get_all_nodes();
    
    // Check all triplets
    for (size_t i = 0; i < all_nodes.size(); ++i) {
        for (size_t j = i + 1; j < all_nodes.size(); ++j) {
            for (size_t k = j + 1; k < all_nodes.size(); ++k) {
                NodeID n1 = all_nodes[i];
                NodeID n2 = all_nodes[j];
                NodeID n3 = all_nodes[k];
                
                if (is_triangle(n1, n2, n3)) {
                    float sum = triangle_weight_sum(n1, n2, n3);
                    if (sum > threshold) {
                        candidates.push_back({n1, n2, n3});
                    }
                }
            }
        }
    }
    
    return candidates;
}

bool LeapNodes::consolidate_triple(NodeID n1, NodeID n2, NodeID n3, float threshold) {
    if (!is_triangle(n1, n2, n3)) {
        return false;
    }
    
    if (triangle_weight_sum(n1, n2, n3) <= threshold) {
        return false;
    }
    
    // Find oldest node (keep this ID)
    NodeID keep_id = get_oldest_node(n1, n2, n3);
    
    // Get the other two nodes to delete
    std::vector<NodeID> to_delete = {n1, n2, n3};
    to_delete.erase(std::remove(to_delete.begin(), to_delete.end(), keep_id), to_delete.end());
    
    // Merge connections: redirect edges from deleted nodes to kept node
    for (NodeID delete_id : to_delete) {
        merge_connections(keep_id, delete_id, to_delete[0] == delete_id ? to_delete[1] : to_delete[0]);
        
        // Remove deleted node
        graph_->remove_node(delete_id);
    }
    
    return true;
}

bool LeapNodes::is_triangle(NodeID n1, NodeID n2, NodeID n3) const {
    return graph_->has_edge(n1, n2) && graph_->has_edge(n1, n3) && 
           graph_->has_edge(n2, n3) && graph_->has_edge(n2, n1) && 
           graph_->has_edge(n3, n1) && graph_->has_edge(n3, n2);
}

float LeapNodes::triangle_weight_sum(NodeID n1, NodeID n2, NodeID n3) const {
    EdgeWeight w12 = graph_->get_edge_weight(n1, n2);
    EdgeWeight w13 = graph_->get_edge_weight(n1, n3);
    EdgeWeight w23 = graph_->get_edge_weight(n2, n3);
    EdgeWeight w21 = graph_->get_edge_weight(n2, n1);
    EdgeWeight w31 = graph_->get_edge_weight(n3, n1);
    EdgeWeight w32 = graph_->get_edge_weight(n3, n2);
    
    // Normalize to [0, 1] range before summing
    float sum = (w12 + w13 + w23 + w21 + w31 + w32) / 65535.0f;
    return sum;
}

NodeID LeapNodes::get_oldest_node(NodeID n1, NodeID n2, NodeID n3) const {
    // Oldest = smallest ID (ascending temporal order)
    return std::min({n1, n2, n3});
}

void LeapNodes::merge_connections(NodeID keep_id, NodeID delete_id1, NodeID delete_id2) const {
    // Redirect all edges from delete_id to keep_id
    graph_->redirect_edge(delete_id1, keep_id);
    graph_->redirect_edge(delete_id2, keep_id);
}

} // namespace melvin

