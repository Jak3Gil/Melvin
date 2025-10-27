#include "MotorNode.h"
#include "../intake/IntakeManager.h"
#include <cstring>

namespace melvin {

MotorNodeData MotorNode::parse_motor_node_data(const melvin::Node* node) {
    MotorNodeData data;
    
    if (node && node->payload_size() >= sizeof(MotorNodeData)) {
        std::memcpy(&data, node->payload(), sizeof(MotorNodeData));
    } else {
        std::memset(&data, 0, sizeof(data));
    }
    
    return data;
}

} // namespace melvin

