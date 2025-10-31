#pragma once

#include "../intake/IntakeManager.h"
#include "../output/OutputManager.h"
#include "../include/melvin/types.h"
#include <vector>

namespace melvin {

// Feeds all outputs back into IntakeManager as new sensory input
class FeedbackRouter {
public:
    FeedbackRouter(IntakeManager* intake, OutputManager* output);
    
    // Route outputs back to intake
    void route_feedback(const std::vector<NodeID>& active_nodes);
    
    // Enable/disable feedback
    void set_enabled(bool enabled) { enabled_ = enabled; }
    bool is_enabled() const { return enabled_; }
    
private:
    IntakeManager* intake_;
    OutputManager* output_;
    bool enabled_;
    
    void feedback_text(const std::vector<NodeID>& text_nodes);
    void feedback_audio(const std::vector<NodeID>& audio_nodes);
    void feedback_visual(const std::vector<NodeID>& vision_nodes);
    void feedback_motor(const std::vector<NodeID>& motor_nodes);
};

} // namespace melvin

