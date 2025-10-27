#pragma once

#include "../core/AtomicGraph.h"
#include "CANInterface.h"
#include "MotorNode.h"
#include "../include/melvin/types.h"
#include <vector>
#include <memory>

namespace melvin {

// Translates graph activations to motor commands
class MotorController {
public:
    MotorController(AtomicGraph* graph, CANInterface* can_interface);
    
    // Process active motor nodes and send commands
    void process_motor_activations(const std::vector<NodeID>& active_nodes);
    
    // Read motor feedback and create intake nodes
    void read_motor_feedback();
    
    // Send command to specific motor
    bool send_motor_command(uint16_t motor_id, const MotorNodeData& command);
    
private:
    AtomicGraph* graph_;
    CANInterface* can_interface_;
    
    // Extract motor nodes from active set
    std::vector<NodeID> extract_motor_nodes(const std::vector<NodeID>& active_nodes);
    
    // Convert graph node to motor command
    MotorNodeData node_to_command(NodeID node);
};

} // namespace melvin

