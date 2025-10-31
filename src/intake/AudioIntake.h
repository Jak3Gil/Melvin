#pragma once

#include <cstdint>

namespace melvin {

class IntakeManager;

class AudioIntake {
public:
    AudioIntake(IntakeManager* manager);
    
    // Capture 20ms @ 16kHz = 640B
    bool capture_audio_chunk(int16_t* buffer);
    
private:
    IntakeManager* manager_;
};

} // namespace melvin

