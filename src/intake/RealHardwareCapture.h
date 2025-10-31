#pragma once
#ifdef __APPLE__

#import "AVFoundationCapture.h"
#include <cstdint>
#include <cstddef>
#include <memory>

namespace melvin {

// Objective-C++ wrapper for real hardware access
class RealHardwareCapture {
public:
    RealHardwareCapture();
    ~RealHardwareCapture();
    
    bool start();
    void stop();
    
    bool get_latest_vision_frame(uint8_t* buffer, size_t buffer_size);
    bool get_latest_audio_chunk(int16_t* buffer, size_t buffer_size);
    
private:
    MelvinCameraCapture* camera_capture_;
    MelvinAudioCapture* audio_capture_;
    bool running_;
};

} // namespace melvin

#endif

