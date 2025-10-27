#include "IntakeManager.h"
#include "VisionIntake.h"
#include "AudioIntake.h"
#include "TextIntake.h"
#include "MotorIntake.h"
#include "../core/Node.h"
#include "../include/melvin/config.h"

namespace melvin {

IntakeManager::IntakeManager(AtomicGraph* graph) 
    : graph_(graph), latest_node_id_(0), total_intakes_(0), next_node_id_(1) {
}

IntakeManager::~IntakeManager() {
    shutdown_all();
}

void IntakeManager::initialize_all() {
    // Initialize intake sources if needed
    vision_intake_ = std::make_unique<VisionIntake>(this);
    audio_intake_ = std::make_unique<AudioIntake>(this);
    text_intake_ = std::make_unique<TextIntake>(this);
    motor_intake_ = std::make_unique<MotorIntake>(this);
}

void IntakeManager::shutdown_all() {
    vision_intake_.reset();
    audio_intake_.reset();
    text_intake_.reset();
    motor_intake_.reset();
}

NodeID IntakeManager::create_vision_node(const uint8_t* pixel_data) {
    NodeID id = allocate_next_id();
    auto node = std::make_unique<Node>(id, pixel_data, VISION_PAYLOAD_SIZE);
    
    if (graph_->add_node(std::move(node))) {
        latest_node_id_ = id;
        total_intakes_.fetch_add(1);
        return id;
    }
    return 0;
}

NodeID IntakeManager::create_audio_node(const int16_t* audio_data) {
    NodeID id = allocate_next_id();
    auto node = std::make_unique<Node>(id, audio_data, AUDIO_PAYLOAD_SIZE);
    
    if (graph_->add_node(std::move(node))) {
        latest_node_id_ = id;
        total_intakes_.fetch_add(1);
        return id;
    }
    return 0;
}

NodeID IntakeManager::create_text_node(char c) {
    NodeID id = allocate_next_id();
    auto node = std::make_unique<Node>(id, &c, TEXT_PAYLOAD_SIZE);
    
    if (graph_->add_node(std::move(node))) {
        latest_node_id_ = id;
        total_intakes_.fetch_add(1);
        return id;
    }
    return 0;
}

NodeID IntakeManager::create_motor_node(uint16_t motor_id, const float* motor_data) {
    NodeID id = allocate_next_id();
    // Motor node includes motor_id + motor_data
    struct MotorPayload {
        uint16_t motor_id;
        float data[7]; // position, velocity, torque, current, voltage, temperature, state
    } payload;
    
    payload.motor_id = motor_id;
    std::memcpy(payload.data, motor_data, sizeof(float) * 7);
    
    auto node = std::make_unique<Node>(id, &payload, sizeof(MotorPayload));
    
    if (graph_->add_node(std::move(node))) {
        latest_node_id_ = id;
        total_intakes_.fetch_add(1);
        return id;
    }
    return 0;
}

NodeID IntakeManager::create_feedback_node(const void* data, size_t size) {
    NodeID id = allocate_next_id();
    auto node = std::make_unique<Node>(id, data, size);
    
    if (graph_->add_node(std::move(node))) {
        latest_node_id_ = id;
        total_intakes_.fetch_add(1);
        return id;
    }
    return 0;
}

NodeID IntakeManager::allocate_next_id() {
    return next_node_id_.fetch_add(1, std::memory_order_relaxed);
}

} // namespace melvin

