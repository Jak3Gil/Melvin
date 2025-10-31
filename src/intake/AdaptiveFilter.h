#pragma once

#include "../include/melvin/types.h"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace melvin {

// Filters input to only capture significant changes
class AdaptiveFilter {
public:
    AdaptiveFilter();
    
    // Check if vision frame should be captured (significant change?)
    bool should_capture_vision(const uint8_t* new_frame, size_t frame_size);
    
    // Check if audio chunk should be captured (enough energy?)
    bool should_capture_audio(const int16_t* new_audio, size_t audio_size);
    
    // Update with new samples (to detect significant changes)
    void update_frame(const uint8_t* frame);
    void update_audio(const int16_t* audio, size_t size);
    
private:
    std::vector<uint8_t> last_frame_;
    std::vector<int16_t> last_audio_;
    
    float calculate_frame_difference(const uint8_t* frame1, const uint8_t* frame2) const;
    float calculate_audio_energy(const int16_t* audio, size_t size) const;
    
    static constexpr float MOTION_THRESHOLD = 0.001f;  // Very low threshold to capture all motion
    static constexpr float AUDIO_THRESHOLD = 0.001f;  // Very low threshold to capture all audio
};

} // namespace melvin

