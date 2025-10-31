#include "NodeAllocator.h"
#include "Node.h"

namespace melvin {

std::unique_ptr<Node> NodeAllocator::allocate_node(const void* payload, size_t payload_size) {
    NodeID id = next_id_.fetch_add(1, std::memory_order_relaxed);
    
    auto node = std::make_unique<Node>(id, payload, payload_size);
    node->set_first_seen(0); // TODO: actual timestamp
    
    std::lock_guard<std::mutex> lock(mutex_);
    NodeID store_id = node->id();
    nodes_[store_id] = std::move(node);
    
    return nullptr; // Node is stored in map, caller doesn't own it
}

Node* NodeAllocator::get_node(NodeID id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nodes_.find(id);
    if (it != nodes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void NodeAllocator::store_node(std::unique_ptr<Node> node) {
    std::lock_guard<std::mutex> lock(mutex_);
    NodeID id = node->id();
    nodes_[id] = std::move(node);
}

void NodeAllocator::collect_unused() {
    // TODO: Implement garbage collection based on frequency/age
    // For now, just clear nodes_ if empty
}

size_t NodeAllocator::node_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return nodes_.size();
}

} // namespace melvin

