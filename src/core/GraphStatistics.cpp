#include "GraphStatistics.h"
#include <iostream>

namespace melvin {

GraphStatistics::GraphStatistics()
    : total_nodes_created_(0), total_nodes_deleted_(0),
      total_edges_created_(0), total_edges_deleted_(0) {
}

void GraphStatistics::node_created() {
    total_nodes_created_.fetch_add(1, std::memory_order_relaxed);
}

void GraphStatistics::node_deleted() {
    total_nodes_deleted_.fetch_add(1, std::memory_order_relaxed);
}

size_t GraphStatistics::get_net_nodes() const {
    size_t created = total_nodes_created_.load();
    size_t deleted = total_nodes_deleted_.load();
    return created > deleted ? created - deleted : 0;
}

void GraphStatistics::edge_created() {
    total_edges_created_.fetch_add(1, std::memory_order_relaxed);
}

void GraphStatistics::edge_deleted() {
    total_edges_deleted_.fetch_add(1, std::memory_order_relaxed);
}

size_t GraphStatistics::get_net_edges() const {
    size_t created = total_edges_created_.load();
    size_t deleted = total_edges_deleted_.load();
    return created > deleted ? created - deleted : 0;
}

void GraphStatistics::reset() {
    total_nodes_created_ = 0;
    total_nodes_deleted_ = 0;
    total_edges_created_ = 0;
    total_edges_deleted_ = 0;
}

void GraphStatistics::print_stats() const {
    size_t net_nodes = get_net_nodes();
    size_t net_edges = get_net_edges();
    
    std::cout << "=== Graph Statistics ===\n";
    std::cout << "Nodes: " << net_nodes << " (created: " << total_nodes_created_.load() 
              << ", deleted: " << total_nodes_deleted_.load() << ")\n";
    std::cout << "Edges: " << net_edges << " (created: " << total_edges_created_.load() 
              << ", deleted: " << total_edges_deleted_.load() << ")\n";
    std::cout << "======================\n";
}

} // namespace melvin

