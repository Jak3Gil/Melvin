#include "Visualizer.h"
#include <iostream>
#include <cstring>

namespace melvin {

Visualizer::Visualizer() {
}

Visualizer::~Visualizer() {
}

void Visualizer::clear_screen() {
    // ANSI escape code to clear screen
    std::cout << "\033[2J\033[H" << std::flush;
}

void Visualizer::show_camera_frame(const uint8_t* frame_data, size_t frame_size) {
    if (!frame_data || frame_size < 768) return;
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ MELVIN CAMERA VIEW (16×16 scaled to 64×32)                │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
    
    // Scale 16×16 to larger display
    for (size_t y = 0; y < 16; ++y) {
        std::cout << "│ ";
        for (size_t x = 0; x < 16; ++x) {
            size_t idx = (y * 16 + x) * 3;
            uint8_t r = frame_data[idx];
            uint8_t g = frame_data[idx + 1];
            uint8_t b = frame_data[idx + 2];
            print_rgb_block(r, g, b);
        }
        std::cout << " │\n";
    }
}

void Visualizer::show_audio_waveform(const int16_t* audio_data, size_t audio_size) {
    if (!audio_data) return;
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ AUDIO WAVEFORM (20ms @ 16kHz)                              │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
    
    // Draw simple waveform (80 chars wide)
    const size_t width = 80;
    std::vector<int> levels(width, 0);
    
    for (size_t i = 0; i < audio_size && i < 320; ++i) {
        int16_t sample = audio_data[i];
        size_t bin = (i * width) / 320;
        if (bin < width) {
            int level = abs(sample) / 1000;
            if (level > levels[bin]) {
                levels[bin] = level;
            }
        }
    }
    
    std::cout << "│";
    for (auto level : levels) {
        std::cout << (level > 20 ? "#" : level > 10 ? ":" : level > 5 ? "." : " ");
    }
    std::cout << "│\n│";
    for (auto level : levels) {
        std::cout << (level > 0 ? "_" : " ");
    }
    std::cout << "│\n";
}

void Visualizer::show_stats(size_t nodes, size_t edges, std::pair<size_t, size_t> attention) {
    std::cout << "\n┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ GRAPH STATISTICS                                            │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Nodes:  " << nodes << "\n";
    std::cout << "│ Edges:  " << edges << "\n";
    std::cout << "│ Attention: 16×16 box at (" << attention.first 
              << ", " << attention.second << ")\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n";
}

void Visualizer::print_rgb_block(uint8_t r, uint8_t g, uint8_t b) {
    // Convert RGB to grayscale for terminal display
    uint8_t gray = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
    const char* ch = " ";
    
    if (gray > 200) ch = "##";
    else if (gray > 150) ch = "##";
    else if (gray > 100) ch = "##";
    else if (gray > 50) ch = "::";
    else if (gray > 25) ch = "..";
    else ch = "  ";
    
    std::cout << ch;
}

char Visualizer::audio_level_to_char(int16_t sample) {
    int level = abs(sample) / 32767.0 * 10;
    if (level > 9) return '#';
    if (level > 6) return ':';
    if (level > 3) return '.';
    return ' ';
}

} // namespace melvin

