#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/types.h"
#include <vector>

namespace melvin {

// Outputs audio nodes to speaker/TTS
class AudioOutput {
public:
    AudioOutput();
    ~AudioOutput();
    
    // Output audio nodes
    void output(const std::vector<NodeID>& audio_nodes, AtomicGraph* graph);
    
    // Initialize audio system
    bool initialize();
    
    // Shutdown audio system
    void shutdown();
    
private:
    bool initialized_;
};

} // namespace melvin

