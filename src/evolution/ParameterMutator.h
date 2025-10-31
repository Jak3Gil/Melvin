#pragma once

#include "../include/melvin/config.h"
#include <random>

namespace melvin {

// Adjusts W, Lw, decay_rate, and thresholds based on fitness trends
class ParameterMutator {
public:
    ParameterMutator();
    
    // Mutate parameters based on fitness
    void mutate_parameters(float fitness);
    
    // Get current parameters
    float get_W() const { return W_; }
    float get_Lw() const { return Lw_; }
    float get_decay_rate() const { return decay_rate_; }
    float get_threshold() const { return threshold_; }
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    float W_;
    float Lw_;
    float decay_rate_;
    float threshold_;
    
    std::mt19937 rng_;
    std::uniform_real_distribution<float> mutation_dist_;
    
    void apply_mutation(float& param, float range);
};

} // namespace melvin

