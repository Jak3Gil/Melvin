#pragma once

#include "../include/melvin/types.h"
#include "../include/melvin/config.h"
#include <vector>
#include <deque>

namespace melvin {

// Combines prediction accuracy, coherence stability, and efficiency into fitness score
class FitnessEvaluator {
public:
    FitnessEvaluator();
    
    // Evaluate fitness for a reasoning cycle
    float evaluate_fitness(float coherence, float accuracy, float efficiency);
    
    // Track fitness history
    void add_fitness_sample(float fitness);
    
    // Get average fitness over window
    float get_average_fitness() const;
    
    // Check if fitness is improving
    bool is_improving() const;
    
    // Reset history
    void reset();
    
private:
    std::deque<float> fitness_history_;
    size_t max_history_size_;
    
    float calculate_trend() const;
};

} // namespace melvin

