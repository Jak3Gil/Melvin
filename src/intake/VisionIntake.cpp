#include "VisionIntake.h"
#include "IntakeManager.h"
#include "../include/melvin/types.h"
#include "../include/melvin/config.h"

namespace melvin {

VisionIntake::VisionIntake(IntakeManager* manager) : manager_(manager) {
}

bool VisionIntake::capture_attention_box(uint8_t* buffer, size_t x, size_t y) {
    // TODO: Implement actual camera capture
    // For now, just create node from buffer
    if (buffer) {
        manager_->create_vision_node(buffer);
        return true;
    }
    return false;
}

} // namespace melvin

