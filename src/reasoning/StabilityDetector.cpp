#include "StabilityDetector.h"

namespace melvin {

StabilityDetector::StabilityDetector(ActivationField* field) : field_(field) {
}

bool StabilityDetector::is_stable(float epsilon) const {
    return field_->is_stable(epsilon);
}

void StabilityDetector::reset() {
    field_->clear();
}

float StabilityDetector::get_variance() const {
    return field_->calculate_variance();
}

} // namespace melvin

