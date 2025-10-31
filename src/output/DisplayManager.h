#pragma once

#include "../include/melvin/types.h"
#include <vector>
#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#endif

namespace melvin {

// Graph statistics for display
struct GraphStats {
    size_t nodes = 0;
    size_t edges = 0;
    float attention_x = 0.0f;
    float attention_y = 0.0f;
    float attention_weight = 0.0f;
};

// Single focus region (one patch being attended to)
struct FocusRegion {
    int patch_x = 0;  // Patch index 0-15
    int patch_y = 0;  // Patch index 0-15
    float score = 0.0f;
};

// Display manager for OpenCV visualization windows
class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    
    // Initialize display windows
    bool init();
    
    // Shutdown display windows
    void shutdown();
    
    // Update camera frame (upscale 16x16 to 256x256 for display)
    void update_camera_frame(const uint8_t* frame_data, size_t frame_size, 
                            const GraphStats& stats, bool show_attention = true);
    
    // Update camera frame with single focus region
    void update_camera_frame_focus(const uint8_t* frame_data, size_t frame_size,
                                   const GraphStats& stats, const FocusRegion& focus);
    
    // Update audio waveform
    void update_audio_waveform(const int16_t* audio_data, size_t audio_size);
    
    // Check if display is active
    bool is_active() const { return active_.load(); }
    
    // Process events (returns false if ESC pressed)
    bool process_events();
    
private:
    std::atomic<bool> active_;
    bool show_overlay_;
    
    // Smooth saccade transition
    float smooth_x_ = 0.0f;
    float smooth_y_ = 0.0f;
    
#ifdef HAVE_OPENCV
    cv::Mat camera_frame_;
    cv::Mat audio_frame_;
    std::mutex frame_mutex_;
    
    // Camera window helpers
    void create_camera_window();
    void draw_camera_overlay(const GraphStats& stats);
    void draw_attention_box(const FocusRegion& focus);
    
    // Audio window helpers
    void create_audio_window();
    void draw_audio_waveform(const int16_t* audio_data, size_t audio_size);
    
    // Scale 16x16 frame to 256x256
    void upscale_frame(const uint8_t* src, cv::Mat& dst);
#else
    // Fallback without OpenCV
    void print_ascii_camera(const uint8_t* frame_data, const GraphStats& stats);
    void print_ascii_audio(const int16_t* audio_data, size_t audio_size);
#endif
};

} // namespace melvin

