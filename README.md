# Melvin - Unified Cognitive Architecture

Melvin is a unified cognitive architecture where all sensory data flows through a single self-organizing graph structure. Nodes represent chunks of sensory or internal data (vision, audio, text, motor, etc.), and connections form purely by co-activation over time.

## Architecture Overview

- **Intake**: Raw data chunked into nodes (768B vision, 640B audio, 1B text, variable motor data)
- **Connections**: Formed by temporal locality and co-activation
- **Generalization**: Leap nodes (consolidate 3 nodes) and leap connections (shortcuts)
- **Reasoning**: Energy-based activation field with coherence-driven traversal
- **Outputs**: Motor control (CAN bus), visual attention (16×16 box), audio, text
- **Evolution**: Adaptive parameter tuning based on fitness metrics
- **Pruning**: Nodes/edges decay based on frequency and age

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+)
- CMake 3.15 or later
- pthread library
- (Optional) SocketCAN for Linux CAN bus support

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
./melvin
```

## Directory Structure

```
Melvin/
├── include/melvin/       # Public headers
│   ├── types.h          # Core type definitions
│   └── config.h         # Configuration constants
├── src/                 # Source files
│   ├── core/            # Graph engine (AtomicGraph, Node, Edge)
│   ├── intake/          # Intake layer (Vision, Audio, Text, Motor)
│   ├── connections/     # Connection formation
│   ├── generalization/  # Leap nodes and connections
│   ├── reasoning/       # Activation, coherence, traversal
│   ├── motor/           # Motor control with CAN interface
│   ├── output/          # Output routing (Audio, Visual, Motor, Text)
│   ├── evolution/       # Fitness evaluation and parameter mutation
│   ├── pruning/         # Node/edge pruning
│   └── feedback/        # Feedback loop
├── tests/               # Unit tests
└── CMakeLists.txt       # Build configuration
```

## Key Components

### Core Graph Engine
- Thread-safe `AtomicGraph` with concurrent node/edge operations
- Variable-size node payloads (vision=768B, audio=640B, text=1B)
- Edge structure: 8B source + 8B target + 2B weight

### Intake Layer
- Vision: 16×16 RGB pixel chunks (768B)
- Audio: 20ms @ 16kHz samples (640B)
- Text: 1 character per node (1B)
- Motor: Kinematic state via CAN bus

### Connection Formation
- Exact connections: Connect to temporal neighbors (before/after)
- Leap nodes: Consolidate 3 fully-connected nodes when sum of weights > W (default 2.5)
- Leap connections: Create shortcuts when 2 nodes → same target with avg weight > Lw (0.51)

### Reasoning
- **Activation Field**: Energy dynamics `E_t = E_(t-1) * decay + Σ(w * neighbor_activation)`
- **Coherence**: Internal vs external weight ratio
- **Traversal**: Combines coherence, edge strength, external relevance
- **Stability**: Stops when energy variance < ε

### Motor Control
- CAN bus interface (SocketCAN on Linux)
- MotorNode with position, velocity, torque, current, voltage, temperature
- Hardware abstraction for multiple motor platforms

### Evolution & Pruning
- Fitness: prediction accuracy + coherence stability + efficiency
- Parameter mutation: W, Lw, decay_rate, thresholds adapt based on fitness
- Pruning: `score = frequency / (1 + age * decay_rate)`

## Configuration

Key constants in `include/melvin/config.h`:

- `DEFAULT_W = 2.5f` - Leap node consolidation threshold
- `DEFAULT_LW = 0.51f` - Leap connection threshold
- `DEFAULT_DECAY_RATE = 0.85f` - Energy decay rate
- `DEFAULT_THRESHOLD = 0.5f` - Activation threshold
- `EXACT_CONNECTION_RADIUS = 10` - Temporal neighbor radius

## License

This project is experimental research software.
