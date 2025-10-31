#include "ActivationField.h"
#include "../include/melvin/types.h"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <unordered_set>

namespace melvin {

ActivationField::ActivationField(AtomicGraph* graph) : graph_(graph) {
}

void ActivationField::set_energy(NodeID node, Energy energy) {
    energies_[node] = energy;
}

Energy ActivationField::get_energy(NodeID node) const {
    auto it = energies_.find(node);
    return it != energies_.end() ? it->second : 0.0f;
}

void ActivationField::update_energies(float decay_rate) {
    // E_t = E_(t-1) * decay + Σ(w * neighbor_activation)
    
    // Create copy of energies for neighbor influence calculation
    std::unordered_map<NodeID, Energy> new_energies = energies_;
    
    for (auto& [node_id, energy] : new_energies) {
        // Decay
        energy *= decay_rate;
        
        // Add neighbor influence
        std::vector<NodeID> neighbors = graph_->get_neighbors(node_id);
        float neighbor_activation = 0.0f;
        
        for (NodeID neighbor_id : neighbors) {
            auto neighbor_it = new_energies.find(neighbor_id);
            if (neighbor_it != new_energies.end()) {
                EdgeWeight weight = graph_->get_edge_weight(node_id, neighbor_id);
                float weight_normalized = static_cast<float>(weight) / 65535.0f;
                neighbor_activation += weight_normalized * neighbor_it->second;
            }
        }
        
        energy += neighbor_activation;
    }
    
    energies_ = std::move(new_energies);
}

std::vector<NodeID> ActivationField::get_active_nodes(float threshold) const {
    std::vector<NodeID> active;
    
    for (const auto& [node_id, energy] : energies_) {
        if (energy > threshold) {
            active.push_back(node_id);
        }
    }
    
    return active;
}

void ActivationField::excite_neighbors(const std::vector<NodeID>& active_nodes) {
    std::unordered_set<NodeID> active_set(active_nodes.begin(), active_nodes.end());
    
    for (NodeID active_node : active_nodes) {
        std::vector<NodeID> neighbors = graph_->get_neighbors(active_node);
        
        for (NodeID neighbor : neighbors) {
            if (active_set.find(neighbor) == active_set.end()) {
                // Neighbor not already active, add excitation
                float current_energy = get_energy(neighbor);
                EdgeWeight weight = graph_->get_edge_weight(active_node, neighbor);
                float excitation = static_cast<float>(weight) / 65535.0f;
                set_energy(neighbor, current_energy + excitation);
            }
        }
    }
}

float ActivationField::calculate_variance() const {
    if (energies_.empty()) {
        return 0.0f;
    }
    
    // Calculate mean
    float sum = std::accumulate(energies_.begin(), energies_.end(), 0.0f,
        [](float acc, const std::pair<NodeID, Energy>& p) {
            return acc + p.second;
        });
    float mean = sum / energies_.size();
    
    // Calculate variance
    float variance = std::accumulate(energies_.begin(), energies_.end(), 0.0f,
        [mean](float acc, const std::pair<NodeID, Energy>& p) {
            float diff = p.second - mean;
            return acc + diff * diff;
        });
    variance /= energies_.size();
    
    return variance;
}

bool ActivationField::is_stable(float epsilon) const {
    return calculate_variance() < epsilon;
}

void ActivationField::clear() {
    energies_.clear();
}

} // namespace melvin

