#pragma once
#include "../include/melvin/types.h"
#include <atomic>
#include <thread>
#include <vector>
#include <memory>

namespace melvin {

// Captures from Mac camera and microphone
class HardwareCapture {
public:
    HardwareCapture();
    ~HardwareCapture();
    
    // Start capturing
    bool start();
    
    // Stop capturing
    void stop();
    
    // Get latest vision frame (768 bytes for 16x16 RGB)
    bool get_latest_vision_frame(uint8_t* buffer, size_t buffer_size);
    
    // Get latest audio chunk (640 bytes for 20ms @ 16kHz)
    bool get_latest_audio_chunk(int16_t* buffer, size_t buffer_size);
    
    // Check if capture is active
    bool is_active() const { return active_.load(); }
    
private:
    std::atomic<bool> active_;
    std::thread camera_thread_;
    std::thread audio_thread_;
    
    // Latest captured data
    std::vector<uint8_t> latest_vision_;
    std::vector<int16_t> latest_audio_;
    std::mutex vision_mutex_;
    std::mutex audio_mutex_;
    
#ifdef __APPLE__
    void* impl_;  // Pointer to HardwareCaptureImpl (hidden to avoid Objective-C in header)
#endif
    
    void capture_camera();
    void capture_audio();
};

} // namespace melvin

