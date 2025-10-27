#pragma once

#include "../../include/melvin/types.h"
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace melvin {

// Forward declaration
class AtomicGraph;

// Base node structure
// Layout: id (8B) + payload (variable)
class Node {
public:
    Node(NodeID id, const void* payload, size_t payload_size) 
        : id_(id), payload_size_(payload_size) {
        if (payload && payload_size > 0) {
            payload_ = new uint8_t[payload_size];
            memcpy(payload_, payload, payload_size);
        } else {
            payload_ = nullptr;
        }
    }
    
    ~Node() {
        if (payload_) {
            delete[] payload_;
        }
    }
    
    // Non-copyable, movable
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    
    Node(Node&& other) noexcept 
        : id_(other.id_), payload_(other.payload_), payload_size_(other.payload_size_) {
        other.payload_ = nullptr;
        other.payload_size_ = 0;
    }
    
    NodeID id() const { return id_; }
    
    const void* payload() const { return payload_; }
    void* payload() { return payload_; }
    
    size_t payload_size() const { return payload_size_; }
    
    // Metadata
    void set_frequency(uint32_t freq) { frequency_ = freq; }
    uint32_t frequency() const { return frequency_; }
    
    void set_first_seen(Time time) { first_seen_ = time; }
    Time first_seen() const { return first_seen_; }
    
    void increment_frequency() { ++frequency_; }
    
private:
    NodeID id_;
    uint8_t* payload_;
    size_t payload_size_;
    uint32_t frequency_ = 1;
    Time first_seen_ = 0;
};

} // namespace melvin

