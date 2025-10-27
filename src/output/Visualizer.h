#pragma once

#include <vector>
#include <cstdint>
#include "../include/melvin/types.h"

namespace melvin {

// Visualizes camera feed and audio waveform in terminal
class Visualizer {
public:
    Visualizer();
    ~Visualizer();
    
    // Show camera frame (16×16 pixels, scaled up to display)
    void show_camera_frame(const uint8_t* frame_data, size_t frame_size);
    
    // Show audio waveform
    void show_audio_waveform(const int16_t* audio_data, size_t audio_size);
    
    // Show stats (nodes, edges, attention position)
    void show_stats(size_t nodes, size_t edges, std::pair<size_t, size_t> attention);
    
    // Clear screen for next frame
    void clear_screen();
    
private:
    void print_rgb_block(uint8_t r, uint8_t g, uint8_t b);
    char audio_level_to_char(int16_t sample);
};

} // namespace melvin

