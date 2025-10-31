#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/types.h"
#include "../include/melvin/config.h"
#include <vector>

namespace melvin {

// Controls 16x16 attention box based on active vision nodes
class VisualAttention {
public:
    VisualAttention();
    
    // Update attention position based on active vision nodes
    void update_attention(const std::vector<NodeID>& vision_nodes, AtomicGraph* graph);
    
    // Get current attention position
    std::pair<size_t, size_t> get_position() const { return {x_, y_}; }
    
    // Set attention position directly
    void set_position(size_t x, size_t y);
    
    // Get attention box size
    std::pair<size_t, size_t> get_box_size() const { return {ATTENTION_BOX_SIZE, ATTENTION_BOX_SIZE}; }
    
    // Print attention position
    void print_attention() const;
    
private:
    size_t x_, y_; // Current attention box position
};

} // namespace melvin

