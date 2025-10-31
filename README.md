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

## 🧠 Core Intelligence System

The brain system has been simplified into a single unified file:

- **`core/melvin_brain.h`**: Complete intelligence system in one file
  - Emergent graph with need-cost connection formation
  - Simple `think()` and `learn()` interface
  - Connections form when: `need (info value + utility + success) > cost (energy + memory)`
  - Automatic strengthening/weakening/pruning

See `core/BRAIN_USAGE_EXAMPLE.md` for usage.

## Building

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+)
- CMake 3.15 or later (for full build) or Make (for production)
- pthread library
- (Optional) SocketCAN for Linux CAN bus support
- **macOS**: AVFoundation, CoreAudio frameworks (included in macOS SDK)

### Build Instructions

#### Full CMake Build:
```bash
mkdir build
cd build
cmake ..
make
./melvin
```

#### Production Jetson Build:
```bash
make clean
make
./bin/melvin_jetson
```

### macOS Setup

**Permissions Required:**
- Camera access (for vision intake)
- Microphone access (for audio intake)

**Grant Permissions:**
1. Open **System Settings → Privacy & Security**
2. Click **Camera** → Enable for Terminal/Cursor/VS Code
3. Click **Microphone** → Enable for Terminal/Cursor/VS Code

**Verify Permissions:**
When you run `./melvin`, you should see:
- ✅ Camera LED turns ON (green indicator)
- Console logs showing "Camera Intake Starting" and "Audio Intake Starting"

**Reset Permissions (if needed):**
```bash
# Reset camera permissions
tccutil reset Camera

# Reset microphone permissions
tccutil reset Microphone
```

**Troubleshooting:**
If camera/microphone don't activate:
1. Check System Settings → Privacy & Security → Camera/Microphone
2. Ensure your terminal app is authorized
3. Run `tccutil reset Camera` and `tccutil reset Microphone` to reset
4. Reboot if needed

## Directory Structure

```
Melvin/
├── core/                    # Core intelligence (simplified)
│   ├── melvin_brain.h       # Unified brain system (single file)
│   ├── emergent_graph.h     # Need-cost connection system
│   └── ...
├── cognitive_os/            # Cognitive OS system
├── include/melvin/          # Public headers
│   ├── types.h             # Core type definitions
│   └── config.h            # Configuration constants
├── src/                     # Source files (full architecture)
│   ├── core/                # Graph engine (AtomicGraph, Node, Edge)
│   ├── intake/              # Intake layer (Vision, Audio, Text, Motor)
│   ├── connections/         # Connection formation
│   ├── generalization/      # Leap nodes and connections
│   ├── reasoning/           # Activation, coherence, traversal
│   ├── motor/               # Motor control with CAN interface
│   ├── output/              # Output routing (Audio, Visual, Motor, Text)
│   ├── evolution/           # Fitness evaluation and parameter mutation
│   ├── pruning/             # Node/edge pruning
│   └── feedback/            # Feedback loop
├── deployment/              # Deployment scripts (Jetson)
├── tests/                   # Unit tests
└── CMakeLists.txt           # Build configuration
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

## Production Deployment (Jetson)

See `JETSON_DEPLOYMENT.md` for full deployment instructions.

Quick deploy:
```bash
cd deployment
./jetson_deploy.sh
```

## Testing

```bash
# Test cognitive OS
./bin/test_cognitive_os

# Run validator
./bin/test_validator --duration 600
```

## License

This project is experimental research software.
