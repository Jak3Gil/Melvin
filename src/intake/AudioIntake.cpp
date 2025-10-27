#include "AudioIntake.h"
#include "IntakeManager.h"
#include "../include/melvin/types.h"

namespace melvin {

AudioIntake::AudioIntake(IntakeManager* manager) : manager_(manager) {
}

bool AudioIntake::capture_audio_chunk(int16_t* buffer) {
    // TODO: Implement actual audio capture
    if (buffer) {
        manager_->create_audio_node(buffer);
        return true;
    }
    return false;
}

} // namespace melvin

