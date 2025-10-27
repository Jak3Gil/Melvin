#pragma once

#include "../core/AtomicGraph.h"
#include "AudioOutput.h"
#include "VisualAttention.h"
#include "MotorOutput.h"
#include "TextOutput.h"
#include "../include/melvin/types.h"
#include <vector>

namespace melvin {

// Routes active nodes to appropriate output channels
class OutputManager {
public:
    OutputManager(AtomicGraph* graph);
    
    // Process active nodes and route to outputs
    void route_outputs(const std::vector<NodeID>& active_nodes);
    
    // Set output handlers
    void set_audio_output(AudioOutput* audio) { audio_output_ = audio; }
    void set_visual_attention(VisualAttention* attention) { visual_attention_ = attention; }
    void set_motor_output(MotorOutput* motor) { motor_output_ = motor; }
    void set_text_output(TextOutput* text) { text_output_ = text; }
    
private:
    AtomicGraph* graph_;
    
    AudioOutput* audio_output_ = nullptr;
    VisualAttention* visual_attention_ = nullptr;
    MotorOutput* motor_output_ = nullptr;
    TextOutput* text_output_ = nullptr;
    
    // Categorize active nodes by type
    void categorize_nodes(const std::vector<NodeID>& active, 
                         std::vector<NodeID>& audio,
                         std::vector<NodeID>& vision,
                         std::vector<NodeID>& motor,
                         std::vector<NodeID>& text);
};

} // namespace melvin

