#pragma once

#include "../core/AtomicGraph.h"
#include "../include/melvin/types.h"
#include "../include/melvin/config.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace melvin {

// Manages energy values and activation dynamics
// E_t = E_(t-1) * decay + Σ(w * neighbor_activation)
class ActivationField {
public:
    ActivationField(AtomicGraph* graph);
    
    // Set/get node energy
    void set_energy(NodeID node, Energy energy);
    Energy get_energy(NodeID node) const;
    
    // Update all active nodes according to energy dynamics
    void update_energies(float decay_rate = DEFAULT_DECAY_RATE);
    
    // Activate nodes above threshold
    std::vector<NodeID> get_active_nodes(float threshold = DEFAULT_THRESHOLD) const;
    
    // Excite neighbors of active nodes
    void excite_neighbors(const std::vector<NodeID>& active_nodes);
    
    // Calculate global energy variance
    float calculate_variance() const;
    
    // Check if system is stable
    bool is_stable(float epsilon = DEFAULT_STABILITY_EPSILON) const;
    
    // Clear all energies
    void clear();
    
private:
    AtomicGraph* graph_;
    std::unordered_map<NodeID, Energy> energies_;
};

} // namespace melvin

