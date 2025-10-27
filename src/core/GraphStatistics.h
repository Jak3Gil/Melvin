#pragma once

#include "../include/melvin/types.h"
#include <atomic>
#include <cstddef>

namespace melvin {

// Tracks total nodes and edges across graph lifetime
class GraphStatistics {
public:
    GraphStatistics();
    
    // Node counting
    void node_created();
    void node_deleted();
    size_t get_total_nodes_created() const { return total_nodes_created_.load(); }
    size_t get_total_nodes_deleted() const { return total_nodes_deleted_.load(); }
    size_t get_net_nodes() const;
    
    // Edge counting
    void edge_created();
    void edge_deleted();
    size_t get_total_edges_created() const { return total_edges_created_.load(); }
    size_t get_total_edges_deleted() const { return total_edges_deleted_.load(); }
    size_t get_net_edges() const;
    
    // Reset counters
    void reset();
    
    // Print statistics
    void print_stats() const;
    
private:
    std::atomic<size_t> total_nodes_created_;
    std::atomic<size_t> total_nodes_deleted_;
    std::atomic<size_t> total_edges_created_;
    std::atomic<size_t> total_edges_deleted_;
};

} // namespace melvin

