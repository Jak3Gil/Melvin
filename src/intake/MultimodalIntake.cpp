#include "MultimodalIntake.h"
#include "../include/melvin/config.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace melvin {

MultimodalIntake::MultimodalIntake(IntakeManager* intake, AtomicGraph* graph)
    : intake_(intake), graph_(graph), running_(false) {
    hardware_ = std::make_unique<HardwareCapture>();
    filter_ = std::make_unique<AdaptiveFilter>();
}

MultimodalIntake::~MultimodalIntake() {
    stop();
}

void MultimodalIntake::start() {
    if (running_.load()) {
        return;
    }
    
    std::cout << "Starting multimodal hardware intake (camera + microphone)...\n";
    
    hardware_->start();
    running_.store(true);
    
    intake_thread_ = std::thread(&MultimodalIntake::continuous_intake_loop, this);
}

void MultimodalIntake::stop() {
    if (!running_.load()) {
        return;
    }
    
    running_.store(false);
    hardware_->stop();
    
    if (intake_thread_.joinable()) {
        intake_thread_.join();
    }
}

void MultimodalIntake::continuous_intake_loop() {
    uint64_t frame_count = 0;
    uint64_t audio_count = 0;
    
    while (running_.load()) {
        // Capture and process vision (only every 3rd attempt to reduce load)
        if (frame_count % 3 == 0) {
            process_vision_frame();
        }
        frame_count++;
        
        // Capture and process audio (only every 3rd attempt to reduce load)
        if (audio_count % 3 == 0) {
            process_audio_chunk();
        }
        audio_count++;
        
        // Throttle to reasonable rate
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Slower to prevent overload
        
        // Print progress less often
        if ((frame_count + audio_count) % 100 == 0) {
            std::cout << "Captured " << frame_count << " vision frames, " 
                      << audio_count << " audio chunks\n";
        }
    }
}

void MultimodalIntake::process_vision_frame() {
    uint8_t buffer[VISION_PAYLOAD_SIZE];
    
    if (hardware_->get_latest_vision_frame(buffer, VISION_PAYLOAD_SIZE)) {
        // Only capture if significant change (adaptive filtering)
        if (filter_->should_capture_vision(buffer, VISION_PAYLOAD_SIZE)) {
            intake_->create_vision_node(buffer);
        }
    }
}

void MultimodalIntake::process_audio_chunk() {
    int16_t buffer[AUDIO_PAYLOAD_SIZE / 2];
    
    if (hardware_->get_latest_audio_chunk(buffer, AUDIO_PAYLOAD_SIZE / 2)) {
        // Only capture if significant energy (adaptive filtering)
        if (filter_->should_capture_audio(buffer, AUDIO_PAYLOAD_SIZE / 2)) {
            intake_->create_audio_node(buffer);
        }
    }
}

} // namespace melvin

