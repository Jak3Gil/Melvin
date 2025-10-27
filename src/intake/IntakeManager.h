#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/config.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

namespace melvin {

// Forward declarations
class VisionIntake;
class AudioIntake;
class TextIntake;
class MotorIntake;

class IntakeManager {
public:
    IntakeManager(AtomicGraph* graph);
    ~IntakeManager();
    
    // Initialize intake sources
    void initialize_all();
    void shutdown_all();
    
    // Thread-safe node creation
    NodeID create_vision_node(const uint8_t* pixel_data);
    NodeID create_audio_node(const int16_t* audio_data);
    NodeID create_text_node(char c);
    NodeID create_motor_node(uint16_t motor_id, const float* motor_data);
    
    // Feedback intake (for closed loop)
    NodeID create_feedback_node(const void* data, size_t size);
    
    // Statistics
    NodeID get_latest_node_id() const { return latest_node_id_.load(); }
    size_t get_total_intakes() const { return total_intakes_.load(); }
    
private:
    AtomicGraph* graph_;
    std::atomic<NodeID> latest_node_id_;
    std::atomic<size_t> total_intakes_;
    
    // Intake sources
    std::unique_ptr<VisionIntake> vision_intake_;
    std::unique_ptr<AudioIntake> audio_intake_;
    std::unique_ptr<TextIntake> text_intake_;
    std::unique_ptr<MotorIntake> motor_intake_;
    
    // Internal node ID counter
    std::atomic<NodeID> next_node_id_;
    
    NodeID allocate_next_id();
};

} // namespace melvin

