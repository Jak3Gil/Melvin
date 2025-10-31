#pragma once

#include "IntakeManager.h"
#include "HardwareCapture.h"
#include "AdaptiveFilter.h"
#include "../core/AtomicGraph.h"
#include <atomic>
#include <thread>

namespace melvin {

// Continuously intakes vision and audio from hardware, feeding both into Melvin
class MultimodalIntake {
public:
    MultimodalIntake(IntakeManager* intake, AtomicGraph* graph);
    ~MultimodalIntake();
    
    // Start continuous intake from hardware
    void start();
    
    // Stop intake
    void stop();
    
    // Check if running
    bool is_running() const { return running_.load(); }
    
private:
    IntakeManager* intake_;
    AtomicGraph* graph_;
    std::unique_ptr<HardwareCapture> hardware_;
    std::unique_ptr<AdaptiveFilter> filter_;
    std::thread intake_thread_;
    std::atomic<bool> running_;
    
    void continuous_intake_loop();
    void process_vision_frame();
    void process_audio_chunk();
};

} // namespace melvin

