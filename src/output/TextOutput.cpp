#include "TextOutput.h"
#include "../core/Node.h"
#include "../include/melvin/config.h"
#include <iostream>

namespace melvin {

TextOutput::TextOutput() {
}

void TextOutput::output(const std::vector<NodeID>& text_nodes, AtomicGraph* graph) {
    for (NodeID node_id : text_nodes) {
        Node* node = graph->get_node(node_id);
        if (node && node->payload_size() == TEXT_PAYLOAD_SIZE) {
            char c = *static_cast<const char*>(node->payload());
            if (c != '\0') {
                buffer_ += c;
                std::cout << c << std::flush;
            }
        }
    }
}

} // namespace melvin

