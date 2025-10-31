#include "MotorOutput.h"
#include "../core/Node.h"
#include "../motor/MotorNode.h"

namespace melvin {

MotorOutput::MotorOutput(MotorController* motor_controller) 
    : motor_controller_(motor_controller) {
}

void MotorOutput::output(const std::vector<NodeID>& motor_nodes, AtomicGraph* graph) {
    if (!motor_controller_) {
        return;
    }
    
    // Send motor commands
    motor_controller_->process_motor_activations(motor_nodes);
}

} // namespace melvin

