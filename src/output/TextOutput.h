#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/types.h"
#include <vector>
#include <string>

namespace melvin {

// Outputs text nodes to terminal/log
class TextOutput {
public:
    TextOutput();
    
    // Output text nodes
    void output(const std::vector<NodeID>& text_nodes, AtomicGraph* graph);
    
    // Get output buffer
    const std::string& get_buffer() const { return buffer_; }
    
    // Clear output buffer
    void clear() { buffer_.clear(); }
    
private:
    std::string buffer_;
};

} // namespace melvin

