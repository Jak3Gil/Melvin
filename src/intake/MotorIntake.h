#pragma once

#include <cstdint>

namespace melvin {

class IntakeManager;

class MotorIntake {
public:
    MotorIntake(IntakeManager* manager);
    
    // Read motor feedback and create node
    bool read_motor_feedback(uint16_t motor_id, const float* motor_data);
    
private:
    IntakeManager* manager_;
};

} // namespace melvin

