#ifdef __APPLE__

#import "RealHardwareCapture.h"
#import "AVFoundationCapture.h"
#import <Foundation/Foundation.h>

namespace melvin {

RealHardwareCapture::RealHardwareCapture() 
    : camera_capture_(nil), audio_capture_(nil), running_(false) {
}

RealHardwareCapture::~RealHardwareCapture() {
    stop();
}

bool RealHardwareCapture::start() {
    if (running_) {
        return false;
    }
    
    @autoreleasepool {
        // Initialize camera
        camera_capture_ = [[MelvinCameraCapture alloc] init];
        if (![camera_capture_ start]) {
            NSLog(@"Failed to start camera");
            camera_capture_ = nil;
        } else {
            NSLog(@"Camera started successfully");
        }
        
        // Initialize audio
        audio_capture_ = [[MelvinAudioCapture alloc] init];
        if (![audio_capture_ start]) {
            NSLog(@"Failed to start audio");
            audio_capture_ = nil;
        } else {
            NSLog(@"Audio started successfully");
        }
    }
    
    running_ = true;
    return true;
}

void RealHardwareCapture::stop() {
    if (!running_) {
        return;
    }
    
    @autoreleasepool {
        if (camera_capture_) {
            [camera_capture_ stop];
            camera_capture_ = nil;
        }
        
        if (audio_capture_) {
            [audio_capture_ stop];
            audio_capture_ = nil;
        }
    }
    
    running_ = false;
}

bool RealHardwareCapture::get_latest_vision_frame(uint8_t* buffer, size_t buffer_size) {
    if (!running_ || !camera_capture_ || buffer_size < 768) {
        return false;
    }
    
    @autoreleasepool {
        uint8_t* frame = [camera_capture_ getLatestFrame:buffer_size];
        if (frame) {
            memcpy(buffer, frame, 768);
            return true;
        }
    }
    
    return false;
}

bool RealHardwareCapture::get_latest_audio_chunk(int16_t* buffer, size_t buffer_size) {
    if (!running_ || !audio_capture_ || buffer_size < 320) {
        return false;
    }
    
    @autoreleasepool {
        int16_t* audio = [audio_capture_ getLatestAudio:buffer_size];
        if (audio) {
            memcpy(buffer, audio, 320 * sizeof(int16_t));
            return true;
        }
    }
    
    return false;
}

} // namespace melvin

#endif

