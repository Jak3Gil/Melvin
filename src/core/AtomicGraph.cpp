#include "AtomicGraph.h"
#include "NodeAllocator.h"
#include <cstring>
#include <functional>

namespace melvin {

AtomicGraph::AtomicGraph() = default;

AtomicGraph::~AtomicGraph() = default;

bool AtomicGraph::add_node(std::unique_ptr<Node> node) {
    NodeID id = node->id();
    
    std::unique_lock<std::shared_mutex> lock(nodes_mutex_);
    
    if (nodes_.find(id) != nodes_.end()) {
        return false; // Already exists
    }
    
    // Add to payload hash map for O(1) lookup
    Node* node_ptr = node.get();
    if (node_ptr && node_ptr->payload_size() > 0) {
        uint64_t hash = hash_payload(node_ptr->payload(), node_ptr->payload_size());
        payload_hash_to_node_[hash] = id;
    }
    
    nodes_[id] = std::move(node);
    return true;
}

Node* AtomicGraph::get_node(NodeID id) {
    std::shared_lock<std::shared_mutex> lock(nodes_mutex_);
    auto it = nodes_.find(id);
    if (it != nodes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool AtomicGraph::remove_node(NodeID id) {
    std::unique_lock<std::shared_mutex> node_lock(nodes_mutex_);
    std::unique_lock<std::shared_mutex> edge_lock(edges_mutex_);
    
    // Remove node
    if (nodes_.erase(id) == 0) {
        return false;
    }
    
    // Remove all edges involving this node
    edges_.erase(id); // outgoing edges
    for (auto& [source_id, targets] : edges_) {
        targets.erase(id); // incoming edges
    }
    
    return true;
}

bool AtomicGraph::add_edge(NodeID source, NodeID target, EdgeWeight weight) {
    std::unique_lock<std::shared_mutex> lock(edges_mutex_);
    edges_[source][target] = weight;
    return true;
}

bool AtomicGraph::remove_edge(NodeID source, NodeID target) {
    std::unique_lock<std::shared_mutex> lock(edges_mutex_);
    auto source_it = edges_.find(source);
    if (source_it != edges_.end()) {
        return source_it->second.erase(target) > 0;
    }
    return false;
}

EdgeWeight AtomicGraph::get_edge_weight(NodeID source, NodeID target) const {
    std::shared_lock<std::shared_mutex> lock(edges_mutex_);
    auto source_it = edges_.find(source);
    if (source_it != edges_.end()) {
        auto target_it = source_it->second.find(target);
        if (target_it != source_it->second.end()) {
            return target_it->second;
        }
    }
    return 0;
}

bool AtomicGraph::has_edge(NodeID source, NodeID target) const {
    std::shared_lock<std::shared_mutex> lock(edges_mutex_);
    auto source_it = edges_.find(source);
    if (source_it != edges_.end()) {
        return source_it->second.find(target) != source_it->second.end();
    }
    return false;
}

std::vector<NodeID> AtomicGraph::get_neighbors(NodeID node) const {
    std::shared_lock<std::shared_mutex> lock(edges_mutex_);
    std::vector<NodeID> neighbors;
    
    auto source_it = edges_.find(node);
    if (source_it != edges_.end()) {
        for (const auto& [target_id, _] : source_it->second) {
            neighbors.push_back(target_id);
        }
    }
    
    // Also check for incoming edges
    for (const auto& [source_id, targets] : edges_) {
        if (targets.find(node) != targets.end()) {
            neighbors.push_back(source_id);
        }
    }
    
    return neighbors;
}

std::vector<Edge> AtomicGraph::get_all_edges() const {
    std::shared_lock<std::shared_mutex> lock(edges_mutex_);
    std::vector<Edge> result;
    
    for (const auto& [source_id, targets] : edges_) {
        for (const auto& [target_id, weight] : targets) {
            result.emplace_back(source_id, target_id, weight);
        }
    }
    
    return result;
}

std::vector<NodeID> AtomicGraph::get_all_nodes() const {
    std::shared_lock<std::shared_mutex> lock(nodes_mutex_);
    std::vector<NodeID> result;
    result.reserve(nodes_.size());
    
    for (const auto& [id, _] : nodes_) {
        result.push_back(id);
    }
    
    return result;
}

bool AtomicGraph::increment_edge_weight(NodeID source, NodeID target, EdgeWeight delta) {
    std::unique_lock<std::shared_mutex> lock(edges_mutex_);
    auto& target_map = edges_[source];
    auto it = target_map.find(target);
    
    if (it != target_map.end()) {
        it->second = std::min<EdgeWeight>(it->second + delta, 65535);
        return true;
    }
    return false;
}

bool AtomicGraph::average_edge_weight(NodeID source, NodeID target, EdgeWeight other_weight) {
    std::unique_lock<std::shared_mutex> lock(edges_mutex_);
    auto& target_map = edges_[source];
    auto it = target_map.find(target);
    
    if (it != target_map.end()) {
        it->second = (it->second + other_weight) / 2;
        return true;
    }
    return false;
}

void AtomicGraph::redirect_edge(NodeID old_target, NodeID new_target) {
    std::unique_lock<std::shared_mutex> lock(edges_mutex_);
    
    // Find all edges to old_target and redirect them
    for (auto& [source_id, targets] : edges_) {
        auto it = targets.find(old_target);
        if (it != targets.end()) {
            EdgeWeight weight = it->second;
            targets.erase(it);
            
            // If new_target already has an edge, average the weights
            if (targets.find(new_target) != targets.end()) {
                targets[new_target] = (targets[new_target] + weight) / 2;
            } else {
                targets[new_target] = weight;
            }
        }
    }
}

size_t AtomicGraph::node_count() const {
    std::shared_lock<std::shared_mutex> lock(nodes_mutex_);
    return nodes_.size();
}

size_t AtomicGraph::edge_count() const {
    std::shared_lock<std::shared_mutex> lock(edges_mutex_);
    size_t count = 0;
    for (const auto& [_, targets] : edges_) {
        count += targets.size();
    }
    return count;
}

void AtomicGraph::clear() {
    std::unique_lock<std::shared_mutex> node_lock(nodes_mutex_);
    std::unique_lock<std::shared_mutex> edge_lock(edges_mutex_);
    nodes_.clear();
    edges_.clear();
}

uint64_t AtomicGraph::hash_payload(const void* payload, size_t size) const {
    std::hash<std::string> hasher;
    std::string data(static_cast<const char*>(payload), size);
    return hasher(data);
}

NodeID AtomicGraph::find_node_with_payload(const void* payload, size_t payload_size) const {
    // Fast O(1) hash lookup
    uint64_t hash = hash_payload(payload, payload_size);
    
    std::shared_lock<std::shared_mutex> lock(payload_hash_mutex_);
    auto it = payload_hash_to_node_.find(hash);
    if (it != payload_hash_to_node_.end()) {
        // Verify it's actually the same (hash collision check)
        std::shared_lock<std::shared_mutex> node_lock(nodes_mutex_);
        auto node_it = nodes_.find(it->second);
        if (node_it != nodes_.end()) {
            const Node* node_ptr = node_it->second.get();
            if (node_ptr && node_ptr->payload_size() == payload_size) {
                if (std::memcmp(node_ptr->payload(), payload, payload_size) == 0) {
                    return node_ptr->id();
                }
            }
        }
    }
    
    return 0; // Not found
}

} // namespace melvin

