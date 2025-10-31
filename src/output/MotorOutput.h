#pragma once

#include "../core/AtomicGraph.h"
#include "../motor/MotorController.h"
#include "../include/melvin/types.h"
#include <vector>

namespace melvin {

// Outputs motor commands based on active motor nodes
class MotorOutput {
public:
    MotorOutput(MotorController* motor_controller);
    
    // Output motor commands
    void output(const std::vector<NodeID>& motor_nodes, AtomicGraph* graph);
    
private:
    MotorController* motor_controller_;
};

} // namespace melvin

