#include "HardwareCapture.h"
#include "../include/melvin/config.h"
#ifdef __APPLE__
#import "AVFoundationCapture.h"
#endif
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>

namespace melvin {

#ifdef __APPLE__
class HardwareCaptureImpl {
public:
    MelvinCameraCapture* camera_capture_;
    MelvinAudioCapture* audio_capture_;
    
    HardwareCaptureImpl() : camera_capture_(nil), audio_capture_(nil) {}
    ~HardwareCaptureImpl() {
        if (camera_capture_) [camera_capture_ release];
        if (audio_capture_) [audio_capture_ release];
    }
    
    bool start() {
        @autoreleasepool {
            camera_capture_ = [[MelvinCameraCapture alloc] init];
            if ([camera_capture_ start]) {
                NSLog(@"✅ Real camera connected!");
                audio_capture_ = [[MelvinAudioCapture alloc] init];
                if ([audio_capture_ start]) {
                    NSLog(@"✅ Real microphone connected!");
                    return true;
                }
            }
            camera_capture_ = nil;
            return false;
        }
    }
};
#endif

HardwareCapture::HardwareCapture() : active_(false) {
#ifdef __APPLE__
    impl_ = nullptr;
#endif
}

HardwareCapture::~HardwareCapture() {
    stop();
}

bool HardwareCapture::start() {
    if (active_.load()) {
        return false;
    }
    
    std::cout << "Starting hardware capture...\n";
    
#ifdef __APPLE__
    // Try to use real hardware
    impl_ = new HardwareCaptureImpl();
    if (static_cast<HardwareCaptureImpl*>(impl_)->start()) {
        std::cout << "✅ Using REAL Mac camera and microphone!\n";
    } else {
        delete static_cast<HardwareCaptureImpl*>(impl_);
        impl_ = nullptr;
        std::cout << "⚠️  Real hardware failed, using simulated data\n";
    }
#endif
    
    // Initialize latest buffers
    latest_vision_.resize(VISION_PAYLOAD_SIZE);
    latest_audio_.resize(AUDIO_PAYLOAD_SIZE / 2);
    
    active_.store(true);
    
    // Start capture threads
    camera_thread_ = std::thread(&HardwareCapture::capture_camera, this);
    audio_thread_ = std::thread(&HardwareCapture::capture_audio, this);
    
    return true;
}

void HardwareCapture::stop() {
    if (!active_.load()) {
        return;
    }
    
    active_.store(false);
    
    if (camera_thread_.joinable()) {
        camera_thread_.join();
    }
    if (audio_thread_.joinable()) {
        audio_thread_.join();
    }
    
#ifdef __APPLE__
    if (impl_) {
        delete static_cast<HardwareCaptureImpl*>(impl_);
        impl_ = nullptr;
    }
#endif
}

bool HardwareCapture::get_latest_vision_frame(uint8_t* buffer, size_t buffer_size) {
    if (!active_.load() || buffer_size < VISION_PAYLOAD_SIZE) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(vision_mutex_);
    
#ifdef __APPLE__
    // Try real hardware first
    if (impl_) {
        HardwareCaptureImpl* hw = static_cast<HardwareCaptureImpl*>(impl_);
        if (hw->camera_capture_) {
            uint8_t* frame = [hw->camera_capture_ getLatestFrame:VISION_PAYLOAD_SIZE];
            if (frame) {
                std::memcpy(buffer, frame, VISION_PAYLOAD_SIZE);
                return true;
            }
        }
    }
#endif
    
    // Try to read from latest_vision_ first
    if (latest_vision_.size() == VISION_PAYLOAD_SIZE) {
        std::memcpy(buffer, latest_vision_.data(), VISION_PAYLOAD_SIZE);
        return true;
    }
    
    // If no captured frame, generate mock data as fallback
    std::srand(std::time(nullptr));
    for (size_t i = 0; i < VISION_PAYLOAD_SIZE && i < buffer_size; ++i) {
        buffer[i] = (uint8_t)(std::rand() % 256);
    }
    
    return true;
}

bool HardwareCapture::get_latest_audio_chunk(int16_t* buffer, size_t buffer_size) {
    if (!active_.load() || buffer_size < AUDIO_PAYLOAD_SIZE / 2) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(audio_mutex_);
    
#ifdef __APPLE__
    // Try real hardware first
    if (impl_) {
        HardwareCaptureImpl* hw = static_cast<HardwareCaptureImpl*>(impl_);
        if (hw->audio_capture_) {
            int16_t* audio = [hw->audio_capture_ getLatestAudio:AUDIO_PAYLOAD_SIZE / 2];
            if (audio) {
                std::memcpy(buffer, audio, AUDIO_PAYLOAD_SIZE / 2 * sizeof(int16_t));
                return true;
            }
        }
    }
#endif
    
    // Try to read from latest_audio_ first
    if (latest_audio_.size() >= AUDIO_PAYLOAD_SIZE / 2) {
        std::memcpy(buffer, latest_audio_.data(), AUDIO_PAYLOAD_SIZE);
        return true;
    }
    
    // If no captured audio, generate mock data as fallback
    std::srand(std::time(nullptr));
    for (size_t i = 0; i < AUDIO_PAYLOAD_SIZE / 2 && i < buffer_size; ++i) {
        buffer[i] = (int16_t)(std::rand() % 32768);
    }
    
    return true;
}

void HardwareCapture::capture_camera() {
    while (active_.load()) {
        {
            std::lock_guard<std::mutex> lock(vision_mutex_);
            
#ifdef __APPLE__
            // Try to get frame from real hardware
            if (impl_) {
                HardwareCaptureImpl* hw = static_cast<HardwareCaptureImpl*>(impl_);
                if (hw->camera_capture_) {
                    uint8_t* frame = [hw->camera_capture_ getLatestFrame:VISION_PAYLOAD_SIZE];
                    if (frame) {
                        // Got real frame!
                        std::memcpy(latest_vision_.data(), frame, VISION_PAYLOAD_SIZE);
                        continue; // Skip simulated data
                    }
                }
            }
#endif
            
            // Fallback: Generate simulated moving pattern
            static size_t frame = 0;
            frame++;
            for (size_t y = 0; y < 16; ++y) {
                for (size_t x = 0; x < 16; ++x) {
                    size_t idx = (y * 16 + x) * 3;
                    latest_vision_[idx + 0] = ((x + frame) % 100) + 50; // R
                    latest_vision_[idx + 1] = ((y + frame) % 100) + 50; // G
                    latest_vision_[idx + 2] = ((x + y + frame) % 100) + 50; // B
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30fps
    }
}

void HardwareCapture::capture_audio() {
    while (active_.load()) {
        {
            std::lock_guard<std::mutex> lock(audio_mutex_);
            
#ifdef __APPLE__
            // Try to get audio from real hardware
            if (impl_) {
                HardwareCaptureImpl* hw = static_cast<HardwareCaptureImpl*>(impl_);
                if (hw->audio_capture_) {
                    int16_t* audio = [hw->audio_capture_ getLatestAudio:AUDIO_PAYLOAD_SIZE / 2];
                    if (audio) {
                        // Got real audio!
                        std::memcpy(latest_audio_.data(), audio, AUDIO_PAYLOAD_SIZE / 2 * sizeof(int16_t));
                        continue; // Skip simulated data
                    }
                }
            }
#endif
            
            // Fallback: Generate simulated sine wave
            static size_t sample = 0;
            for (size_t i = 0; i < AUDIO_PAYLOAD_SIZE / 2; ++i) {
                double phase = (sample + i) * 0.01;
                latest_audio_[i] = static_cast<int16_t>(sin(phase) * 10000);
                sample++;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 20ms chunks
    }
}

} // namespace melvin

