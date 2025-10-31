#include "EvolutionEngine.h"

namespace melvin {

EvolutionEngine::EvolutionEngine() : cycle_count_(0) {
}

void EvolutionEngine::update(float coherence, float accuracy, float efficiency) {
    // Evaluate fitness
    float fitness = fitness_evaluator_.evaluate_fitness(coherence, accuracy, efficiency);
    
    // Track history
    fitness_evaluator_.add_fitness_sample(fitness);
    
    // Mutate parameters based on fitness
    mutator_.mutate_parameters(fitness);
    
    cycle_count_++;
}

float EvolutionEngine::get_average_fitness() const {
    return fitness_evaluator_.get_average_fitness();
}

bool EvolutionEngine::is_improving() const {
    return fitness_evaluator_.is_improving();
}

} // namespace melvin

