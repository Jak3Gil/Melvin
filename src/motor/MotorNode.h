#pragma once

#include "../core/Node.h"
#include "../include/melvin/types.h"
#include <cstdint>

namespace melvin {

// Extended motor node structure
// Includes motor_id (CAN ID) + motor state data
struct MotorNodeData {
    uint16_t motor_id;          // CAN hardware ID
    float position;              // Current joint angle (radians)
    float velocity;              // Angular velocity (rad/s)
    float torque;                // Output torque (Nm)
    float current;               // Motor current (A)
    float voltage;               // Supply voltage (V)
    float temperature;           // Internal temperature (°C)
    uint64_t timestamp;          // Time in ms
    uint8_t state;              // 0=idle, 1=moving, 2=error
};

// Helper class for motor node operations
class MotorNode {
public:
    // Create motor node from data
    static melvin::NodeID create_motor_node(AtomicGraph* graph, const MotorNodeData& data);
    
    // Parse motor node data from raw payload
    static MotorNodeData parse_motor_node_data(const melvin::Node* node);
    
    // Update motor feedback data
    static void update_feedback(MotorNodeData& data, float position, float velocity, float current);
};

} // namespace melvin

