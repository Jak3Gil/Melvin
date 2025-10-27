#include "MotorIntake.h"
#include "IntakeManager.h"
#include "../include/melvin/types.h"

namespace melvin {

MotorIntake::MotorIntake(IntakeManager* manager) : manager_(manager) {
}

bool MotorIntake::read_motor_feedback(uint16_t motor_id, const float* motor_data) {
    // TODO: Implement actual CAN bus read
    if (motor_data) {
        return manager_->create_motor_node(motor_id, motor_data) != 0;
    }
    return false;
}

} // namespace melvin

