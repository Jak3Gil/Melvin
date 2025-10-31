#pragma once

#include "../include/melvin/types.h"

namespace melvin {

class Weight {
public:
    // Update weight based on co-activation
    static EdgeWeight update_by_coactivation(EdgeWeight current, EdgeWeight neighbor_activation);
    
    // Decay weight over time
    static EdgeWeight decay(EdgeWeight weight, float decay_rate);
    
    // Normalize weight to [0, 65535]
    static EdgeWeight normalize(float weight);
};

} // namespace melvin

