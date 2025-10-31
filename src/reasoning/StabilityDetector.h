#pragma once

#include "ActivationField.h"
#include "../include/melvin/config.h"

namespace melvin {

// Detects when reasoning has reached a stable attractor state
class StabilityDetector {
public:
    StabilityDetector(ActivationField* field);
    
    // Check if system is stable
    bool is_stable(float epsilon = DEFAULT_STABILITY_EPSILON) const;
    
    // Reset stability tracking
    void reset();
    
    // Get current variance
    float get_variance() const;
    
private:
    ActivationField* field_;
};

} // namespace melvin

