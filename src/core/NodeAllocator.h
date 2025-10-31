#pragma once

#include "../include/melvin/types.h"
#include "Node.h"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <atomic>

namespace melvin {

class NodeAllocator {
public:
    static NodeAllocator& get_instance() {
        static NodeAllocator instance;
        return instance;
    }
    
    // Allocate a new node with ascending ID
    std::unique_ptr<Node> allocate_node(const void* payload, size_t payload_size);
    
    // Get node by ID (thread-safe)
    Node* get_node(NodeID id);
    
    // Store node
    void store_node(std::unique_ptr<Node> node);
    
    // Garbage collection
    void collect_unused();
    
    size_t node_count() const;
    
private:
    NodeAllocator() : next_id_(1) {}
    ~NodeAllocator() = default;
    
    NodeAllocator(const NodeAllocator&) = delete;
    NodeAllocator& operator=(const NodeAllocator&) = delete;
    
    mutable std::mutex mutex_;
    std::atomic<NodeID> next_id_;
    std::unordered_map<NodeID, std::unique_ptr<Node>> nodes_;
};

} // namespace melvin

