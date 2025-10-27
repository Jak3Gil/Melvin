#pragma once

#include "FitnessEvaluator.h"
#include "ParameterMutator.h"
#include "../include/melvin/types.h"
#include <memory>

namespace melvin {

// Manages mutation cycles and tracks fitness history
class EvolutionEngine {
public:
    EvolutionEngine();
    
    // Evaluate current cycle and update parameters
    void update(float coherence, float accuracy, float efficiency);
    
    // Get current parameters
    float get_W() const { return mutator_.get_W(); }
    float get_Lw() const { return mutator_.get_Lw(); }
    float get_decay_rate() const { return mutator_.get_decay_rate(); }
    float get_threshold() const { return mutator_.get_threshold(); }
    
    // Statistics
    float get_average_fitness() const;
    bool is_improving() const;
    
private:
    FitnessEvaluator fitness_evaluator_;
    ParameterMutator mutator_;
    size_t cycle_count_;
};

} // namespace melvin

