#include "MotorController.h"
#include "../core/Node.h"
#include "../motor/MotorNode.h"
#include <algorithm>
#include <cstring>

namespace melvin {

MotorController::MotorController(AtomicGraph* graph, CANInterface* can_interface)
    : graph_(graph), can_interface_(can_interface) {
}

void MotorController::process_motor_activations(const std::vector<NodeID>& active_nodes) {
    std::vector<NodeID> motor_nodes = extract_motor_nodes(active_nodes);
    
    for (NodeID motor_node : motor_nodes) {
        MotorNodeData command = node_to_command(motor_node);
        send_motor_command(command.motor_id, command);
    }
}

void MotorController::read_motor_feedback() {
    if (!can_interface_->is_open()) {
        return;
    }
    
    // Read all available CAN frames
    while (true) {
        uint16_t motor_id;
        uint8_t data[8];
        size_t len;
        
        if (!can_interface_->read_frame(&motor_id, data, &len)) {
            break;
        }
        
        // Parse feedback data
        MotorNodeData feedback;
        feedback.motor_id = motor_id;
        std::memcpy(&feedback.position, data, sizeof(float));
        std::memcpy(&feedback.velocity, data + sizeof(float), sizeof(float));
        std::memcpy(&feedback.current, data + 2 * sizeof(float), sizeof(float));
        
        // TODO: Call intake manager to create motor node
    }
}

bool MotorController::send_motor_command(uint16_t motor_id, const MotorNodeData& command) {
    if (!can_interface_->is_open()) {
        return false;
    }
    
    uint8_t data[8];
    std::memcpy(data, &command.position, sizeof(float));
    std::memcpy(data + sizeof(float), &command.velocity, sizeof(float));
    std::memcpy(data + 2 * sizeof(float), &command.torque, sizeof(float));
    std::memcpy(data + 3 * sizeof(float), &command.current, sizeof(float));
    
    return can_interface_->write_frame(motor_id, data, 4 * sizeof(float));
}

std::vector<NodeID> MotorController::extract_motor_nodes(const std::vector<NodeID>& active_nodes) {
    std::vector<NodeID> motor_nodes;
    
    for (NodeID node_id : active_nodes) {
        Node* node = graph_->get_node(node_id);
        if (node && node->payload_size() >= sizeof(MotorNodeData)) {
            motor_nodes.push_back(node_id);
        }
    }
    
    return motor_nodes;
}

MotorNodeData MotorController::node_to_command(NodeID node) {
    Node* node_ptr = graph_->get_node(node);
    if (!node_ptr) {
        return MotorNodeData{};
    }
    
    MotorNodeData data = MotorNode::parse_motor_node_data(node_ptr);
    // TODO: Adjust command based on current state
    
    return data;
}

} // namespace melvin

