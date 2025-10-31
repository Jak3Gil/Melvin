#include "OutputManager.h"
#include "../core/Node.h"
#include "../include/melvin/types.h"
#include "../include/melvin/config.h"
#include <iostream>

namespace melvin {

OutputManager::OutputManager(AtomicGraph* graph) : graph_(graph) {
}

void OutputManager::route_outputs(const std::vector<NodeID>& active_nodes) {
    std::vector<NodeID> audio, vision, motor, text;
    categorize_nodes(active_nodes, audio, vision, motor, text);
    
    if (!audio.empty() && audio_output_) {
        audio_output_->output(audio, graph_);
    }
    
    if (!vision.empty() && visual_attention_) {
        visual_attention_->update_attention(vision, graph_);
        std::cout << "Vision nodes active: " << vision.size() << " - ";
        visual_attention_->print_attention();
    }
    
    if (!motor.empty() && motor_output_) {
        motor_output_->output(motor, graph_);
    }
    
    if (!text.empty() && text_output_) {
        text_output_->output(text, graph_);
    }
}

void OutputManager::categorize_nodes(const std::vector<NodeID>& active,
                                      std::vector<NodeID>& audio,
                                      std::vector<NodeID>& vision,
                                      std::vector<NodeID>& motor,
                                      std::vector<NodeID>& text) {
    audio.clear();
    vision.clear();
    motor.clear();
    text.clear();
    
    for (NodeID node_id : active) {
        Node* node = graph_->get_node(node_id);
        if (!node) continue;
        
        size_t payload_size = node->payload_size();
        
        if (payload_size == AUDIO_PAYLOAD_SIZE) {
            audio.push_back(node_id);
        } else if (payload_size == VISION_PAYLOAD_SIZE) {
            vision.push_back(node_id);
        } else if (payload_size >= sizeof(melvin::MotorNodeData)) {
            motor.push_back(node_id);
        } else if (payload_size == TEXT_PAYLOAD_SIZE) {
            text.push_back(node_id);
        }
    }
}

} // namespace melvin

