#include "FitnessEvaluator.h"
#include "../include/melvin/config.h"
#include <numeric>
#include <algorithm>
#include <deque>

namespace melvin {

FitnessEvaluator::FitnessEvaluator() : max_history_size_(FITNESS_WINDOW_SIZE) {
}

float FitnessEvaluator::evaluate_fitness(float coherence, float accuracy, float efficiency) {
    // Weighted combination
    float fitness = coherence * 0.4f + accuracy * 0.4f + efficiency * 0.2f;
    return fitness;
}

void FitnessEvaluator::add_fitness_sample(float fitness) {
    fitness_history_.push_back(fitness);
    if (fitness_history_.size() > max_history_size_) {
        fitness_history_.pop_front();
    }
}

float FitnessEvaluator::get_average_fitness() const {
    if (fitness_history_.empty()) {
        return 0.0f;
    }
    
    float sum = std::accumulate(fitness_history_.begin(), fitness_history_.end(), 0.0f);
    return sum / fitness_history_.size();
}

bool FitnessEvaluator::is_improving() const {
    return calculate_trend() > 0.0f;
}

void FitnessEvaluator::reset() {
    fitness_history_.clear();
}

float FitnessEvaluator::calculate_trend() const {
    if (fitness_history_.size() < 2) {
        return 0.0f;
    }
    
    // Linear regression slope
    float sum_x = 0.0f, sum_y = 0.0f, sum_xy = 0.0f, sum_x2 = 0.0f;
    size_t n = fitness_history_.size();
    
    for (size_t i = 0; i < n; ++i) {
        float x = static_cast<float>(i);
        float y = fitness_history_[i];
        
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
    }
    
    float denominator = n * sum_x2 - sum_x * sum_x;
    if (denominator == 0.0f) {
        return 0.0f;
    }
    
    float slope = (n * sum_xy - sum_x * sum_y) / denominator;
    return slope;
}

} // namespace melvin

