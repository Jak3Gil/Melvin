#include "Weight.h"

namespace melvin {

EdgeWeight Weight::update_by_coactivation(EdgeWeight current, EdgeWeight neighbor_activation) {
    // Simple update: average current weight with neighbor activation
    float updated = (static_cast<float>(current) + static_cast<float>(neighbor_activation)) / 2.0f;
    return static_cast<EdgeWeight>(std::min<float>(updated, 65535.0f));
}

EdgeWeight Weight::decay(EdgeWeight weight, float decay_rate) {
    return static_cast<EdgeWeight>(static_cast<float>(weight) * decay_rate);
}

EdgeWeight Weight::normalize(float weight) {
    // Normalize to uint16_t range [0, 65535]
    float clamped = std::max<float>(0.0f, std::min<float>(1.0f, weight));
    return static_cast<EdgeWeight>(clamped * 65535.0f);
}

} // namespace melvin

