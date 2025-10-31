#pragma once

#include <cstdint>
#include <cstddef>

namespace melvin {

// Generalization thresholds
constexpr float DEFAULT_W = 2.5f;   // Leap node consolidation threshold
constexpr float DEFAULT_LW = 0.51f; // Leap connection threshold

// Activation dynamics
constexpr float DEFAULT_DECAY_RATE = 0.85f;    // Energy decay per timestep
constexpr float DEFAULT_THRESHOLD = 0.5f;      // Activation threshold θ
constexpr float DEFAULT_STABILITY_EPSILON = 0.01f; // Energy variance threshold ε

// Connection formation
constexpr size_t EXACT_CONNECTION_RADIUS = 10; // Connect to X nodes before/after

// Initial edge weight
constexpr uint16_t INITIAL_EDGE_WEIGHT = 100;  // Max is 65535

// Pruning parameters
constexpr float PRUNING_DECAY_RATE = 0.001f;
constexpr float PRUNING_THRESHOLD = 0.1f;

// Evolution parameters
constexpr float MUTATION_RATE = 0.05f;
constexpr size_t FITNESS_WINDOW_SIZE = 100;  // Cycles to track for fitness

// Reasoning/coherence
constexpr size_t MAX_REASONING_ITERATIONS = 100;
constexpr float MIN_COHERENCE_RATIO = 1.2f;  // Internal/external weight ratio

// Motor control
constexpr float MOTOR_CONTROL_RATE = 50.0f;  // Hz
constexpr uint16_t MAX_MOTORS = 32;

// Visual attention
constexpr size_t ATTENTION_BOX_SIZE = 16;  // 16x16 pixels

} // namespace melvin

