#include "VisualAttention.h"
#include "../core/Node.h"
#include "../include/melvin/config.h"
#include <iostream>
#include <cstring>

namespace melvin {

VisualAttention::VisualAttention() : x_(0), y_(0) {
}

void VisualAttention::update_attention(const std::vector<NodeID>& vision_nodes, AtomicGraph* graph) {
    if (vision_nodes.empty()) {
        return;
    }
    
    // Calculate average position from active vision nodes
    // In a real system, this would analyze image content
    // For now, move attention toward center of active nodes
    size_t sum_x = 0, sum_y = 0;
    size_t count = 0;
    
    for (NodeID node_id : vision_nodes) {
        Node* node = graph->get_node(node_id);
        if (node && node->payload_size() == VISION_PAYLOAD_SIZE) {
            // Extract position from metadata if available
            // For now, just increment
            sum_x += node_id % 100;  // Mock calculation
            sum_y += (node_id / 100) % 100;
            count++;
        }
    }
    
    if (count > 0) {
        x_ = sum_x / count;
        y_ = sum_y / count;
    }
}

void VisualAttention::set_position(size_t x, size_t y) {
    x_ = x;
    y_ = y;
}

void VisualAttention::print_attention() const {
    std::cout << "  Attention: 16×16 box at (" << x_ << ", " << y_ << ")\n";
}

} // namespace melvin

