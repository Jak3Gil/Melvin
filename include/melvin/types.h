#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>

namespace melvin {

// Core type definitions
using NodeID = uint64_t;
using EdgeWeight = uint16_t;
using Energy = float;
using Time = uint64_t;  // milliseconds since epoch or boot

// Fixed payload sizes by modality
constexpr size_t VISION_PAYLOAD_SIZE = 768;  // 16x16 RGB
constexpr size_t AUDIO_PAYLOAD_SIZE = 640;   // 20ms @ 16kHz = 320 samples × 2B
constexpr size_t TEXT_PAYLOAD_SIZE = 1;      // 1 character
constexpr size_t MOTOR_PAYLOAD_SIZE = sizeof(float) * 8;  // position, velocity, torque, current, voltage, temperature, + state bytes

// Maximum payload size for allocation
constexpr size_t MAX_PAYLOAD_SIZE = VISION_PAYLOAD_SIZE;

// Node states
enum class NodeState : uint8_t {
    INACTIVE = 0,
    ACTIVE = 1,
    STABLE = 2
};

// Edge structure: 8B source + 8B target + 2B weight = 18B
struct Edge {
    NodeID source;
    NodeID target;
    EdgeWeight weight;
    
    Edge(NodeID s, NodeID t, EdgeWeight w) : source(s), target(t), weight(w) {}
    
    bool operator<(const Edge& other) const {
        if (source != other.source) return source < other.source;
        return target < other.target;
    }
    
    bool operator==(const Edge& other) const {
        return source == other.source && target == other.target;
    }
};

// Connection frequencies for pruning
struct ConnectionFrequency {
    Time first_seen;
    uint32_t frequency;
    
    ConnectionFrequency() : first_seen(0), frequency(0) {}
};

} // namespace melvin

