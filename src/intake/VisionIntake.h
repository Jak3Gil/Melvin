#pragma once

#include <cstdint>
#include <cstddef>

namespace melvin {

class IntakeManager;

class VisionIntake {
public:
    VisionIntake(IntakeManager* manager);
    
    // Capture 16x16 pixel box
    bool capture_attention_box(uint8_t* buffer, size_t x, size_t y);
    
private:
    IntakeManager* manager_;
};

} // namespace melvin

