#pragma once

#include "AtomicGraph.h"
#include "../include/melvin/types.h"
#include <string>

namespace melvin {

// Saves and loads graph to/from binary files
class BinaryPersistence {
public:
    BinaryPersistence(AtomicGraph* graph);
    
    // Save graph to binary files
    bool save_to_files(const std::string& nodes_file = "data/nodes.bin",
                       const std::string& edges_file = "data/edges.bin");
    
    // Load graph from binary files
    bool load_from_files(const std::string& nodes_file = "data/nodes.bin",
                         const std::string& edges_file = "data/edges.bin");
    
    // Get statistics
    size_t get_nodes_count() const { return nodes_count_; }
    size_t get_edges_count() const { return edges_count_; }
    
private:
    AtomicGraph* graph_;
    size_t nodes_count_ = 0;
    size_t edges_count_ = 0;
    
    struct NodeRecord {
        NodeID id;
        uint32_t frequency;
        Time first_seen;
        size_t payload_size;
        // payload follows
    };
    
    struct EdgeRecord {
        NodeID source;
        NodeID target;
        EdgeWeight weight;
    };
};

} // namespace melvin

