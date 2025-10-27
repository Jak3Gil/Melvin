#include "ParameterMutator.h"
#include "../include/melvin/config.h"
#include <cmath>
#include <random>

namespace melvin {

ParameterMutator::ParameterMutator() 
    : W_(DEFAULT_W), Lw_(DEFAULT_LW), decay_rate_(DEFAULT_DECAY_RATE), threshold_(DEFAULT_THRESHOLD),
      rng_(std::random_device{}()), mutation_dist_(-MUTATION_RATE, MUTATION_RATE) {
}

void ParameterMutator::mutate_parameters(float fitness) {
    // If fitness is good, stabilize; otherwise mutate
    float mutation_factor = fitness < 0.7f ? MUTATION_RATE : 0.0f;
    
    apply_mutation(W_, mutation_factor);
    apply_mutation(Lw_, mutation_factor);
    apply_mutation(decay_rate_, mutation_factor);
    apply_mutation(threshold_, mutation_factor);
}

void ParameterMutator::apply_mutation(float& param, float range) {
    if (range > 0.0f) {
        float mutation = mutation_dist_(rng_) * range;
        param += mutation;
        
        // Clamp to reasonable bounds
        if (param < 0.0f) param = 0.0f;
        if (param > 10.0f) param = 10.0f;
    }
}

void ParameterMutator::reset_to_defaults() {
    W_ = DEFAULT_W;
    Lw_ = DEFAULT_LW;
    decay_rate_ = DEFAULT_DECAY_RATE;
    threshold_ = DEFAULT_THRESHOLD;
}

} // namespace melvin

