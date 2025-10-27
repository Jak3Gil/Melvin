#include "AdaptiveFilter.h"
#include <cmath>
#include <cstring>
#include <algorithm>

namespace melvin {

AdaptiveFilter::AdaptiveFilter() {
    last_frame_.resize(768);  // VISION_PAYLOAD_SIZE
    last_audio_.resize(320);  // AUDIO_PAYLOAD_SIZE / 2
}

bool AdaptiveFilter::should_capture_vision(const uint8_t* new_frame, size_t frame_size) {
    if (!new_frame || frame_size < 768) return false;
    
    // Check if significant motion
    if (last_frame_.empty()) {
        update_frame(new_frame);
        return true;  // First frame always capture
    }
    
    float difference = calculate_frame_difference(new_frame, last_frame_.data());
    
    if (difference > MOTION_THRESHOLD) {
        update_frame(new_frame);
        return true;
    }
    
    return false;
}

bool AdaptiveFilter::should_capture_audio(const int16_t* new_audio, size_t audio_size) {
    if (!new_audio || audio_size < 320) return false;
    
    // Check if significant audio energy
    float energy = calculate_audio_energy(new_audio, audio_size);
    
    if (energy > AUDIO_THRESHOLD) {
        // Update last audio
        size_t copy_size = std::min(audio_size, (size_t)320);
        std::memcpy(last_audio_.data(), new_audio, copy_size * sizeof(int16_t));
        return true;
    }
    
    return false;
}

void AdaptiveFilter::update_frame(const uint8_t* frame) {
    std::memcpy(last_frame_.data(), frame, 768);
}

void AdaptiveFilter::update_audio(const int16_t* audio, size_t size) {
    size_t copy_size = std::min(size, (size_t)320);
    std::memcpy(last_audio_.data(), audio, copy_size * sizeof(int16_t));
}

float AdaptiveFilter::calculate_frame_difference(const uint8_t* frame1, const uint8_t* frame2) const {
    float total_diff = 0.0f;
    size_t pixels = 16 * 16;
    
    for (size_t i = 0; i < pixels * 3; ++i) {
        int diff = abs((int)frame1[i] - (int)frame2[i]);
        total_diff += (float)diff / 255.0f;
    }
    
    return total_diff / (pixels * 3);  // Average difference per pixel
}

float AdaptiveFilter::calculate_audio_energy(const int16_t* audio, size_t size) const {
    float energy = 0.0f;
    
    for (size_t i = 0; i < size && i < 320; ++i) {
        float sample = (float)audio[i] / 32768.0f;
        energy += sample * sample;
    }
    
    return energy / size;  // RMS energy
}

} // namespace melvin

