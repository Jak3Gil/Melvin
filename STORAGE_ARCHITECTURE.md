# Melvin Data Storage Architecture

## Overview

All nodes and connections are stored **in-memory** within the `AtomicGraph` object. There is currently no persistence to disk - the graph exists only while the program is running.

## Storage Locations

### Primary Storage: AtomicGraph

The `AtomicGraph` class (defined in `src/core/AtomicGraph.h`) contains two main data structures:

```cpp
class AtomicGraph {
private:
    // NODES: Map from NodeID -> unique_ptr<Node>
    std::unordered_map<NodeID, std::unique_ptr<Node>> nodes_;
    
    // EDGES: Nested map from source NodeID -> (target NodeID -> weight)
    std::unordered_map<NodeID, std::unordered_map<NodeID, EdgeWeight>> edges_;
    
    // Thread safety mutexes
    mutable std::shared_mutex nodes_mutex_;
    mutable std::shared_mutex edges_mutex_;
};
```

### Memory Layout

#### Nodes Storage Structure

Each node is stored with:
- **Node ID** (uint64_t): Unique ascending identifier (8 bytes)
- **Payload** (uint8_t*): Variable-size data block
  - Vision: 768 bytes (16×16 RGB pixels)
  - Audio: 640 bytes (320 samples × 2 bytes)
  - Text: 1 byte (single character)
  - Motor: variable size
- **Frequency** (uint32_t): Usage count
- **First Seen** (Time): Timestamp

```cpp
// Location: src/core/Node.h
class Node {
    NodeID id_;                    // 8 bytes
    uint8_t* payload_;             // Variable (1B-768B)
    size_t payload_size_;          // Size of payload
    uint32_t frequency_ = 1;       // 4 bytes
    Time first_seen_ = 0;           // 8 bytes
};
```

#### Edges Storage Structure

Edges are stored as:
```cpp
// Source ID -> (Target ID -> Weight)
edges_[source_node_id][target_node_id] = edge_weight
```

Edge data:
- **Source NodeID** (uint64_t): 8 bytes
- **Target NodeID** (uint64_t): 8 bytes  
- **Weight** (uint16_t): 2 bytes (0-65535)
- **Total per edge**: 18 bytes in the structure

However, the actual storage uses:
```cpp
std::unordered_map<NodeID, std::unordered_map<NodeID, EdgeWeight>>
```

This nested hash map is more memory-efficient for sparse graphs.

## Memory Location

All data is stored in **heap memory** allocated at runtime:

1. **AtomicGraph instance** - Created in `main.cpp` on the stack
2. **Node objects** - Allocated in heap via `std::make_unique<Node>()`
3. **Node payloads** - Heap-allocated arrays (via `new uint8_t[size]`)
4. **Edge maps** - Heap storage within `std::unordered_map`

## Example: Loading "Hello"

When you load the text "Hello":

```
NodeID 1: 'H' (1 byte payload) -> nodes_[1]
NodeID 2: 'e' (1 byte payload) -> nodes_[2]
NodeID 3: 'l' (1 byte payload) -> nodes_[3]
NodeID 4: 'l' (1 byte payload) -> nodes_[4]
NodeID 5: 'o' (1 byte payload) -> nodes_[5]

// Connections form between temporal neighbors (radius=10)
edges_[1][2] = 100  // H connects to e
edges_[2][1] = 100  // e connects back to H
edges_[2][3] = 100  // e connects to l
edges_[3][2] = 100  // etc...
```

## Memory Estimation

For your current dataset (~30,000 characters):

**Nodes:**
- 30,000 nodes × (8B id + ~1B payload + 4B frequency + 8B timestamp) ≈ 630KB for nodes
- Plus overhead from unordered_map ≈ ~1MB

**Edges:**
- With radius=10 neighbors per node: ~20 neighbors × 2 (bidirectional)
- 30,000 nodes × 40 edges ≈ 1,200,000 edges
- Edge storage: ~2 bytes weight + ~8B overhead per edge ≈ 30MB

**Total estimate:** ~30-40MB in memory

## Access Patterns

### Reading Nodes
```cpp
// From main.cpp - graph is the AtomicGraph instance
auto all_nodes = graph->get_all_nodes();  // Returns vector of NodeIDs

// Get specific node
Node* node = graph->get_node(node_id);

// Read payload
const uint8_t* data = node->payload();
size_t size = node->payload_size();
```

### Reading Edges
```cpp
// Get neighbors of a node
std::vector<NodeID> neighbors = graph->get_neighbors(node_id);

// Get edge weight
EdgeWeight w = graph->get_edge_weight(source_id, target_id);

// Get all edges
std::vector<Edge> all_edges = graph->get_all_edges();
```

## Current Limitation: No Persistence

⚠️ **Important**: The current implementation has NO disk persistence. When the program exits, all data is lost.

To add persistence, you would need to implement:
1. Serialization to binary file (save nodes + edges)
2. Deserialization on startup
3. Optional: Periodic checkpoints

## Thread Safety

Both node and edge storage use read-write locks for thread-safe access:
- Multiple readers can access simultaneously (`shared_lock`)
- Writers get exclusive access (`unique_lock`)
- This allows concurrent reads with occasional writes

## Memory Management

- **Node creation**: Via `IntakeManager::create_*_node()` methods
- **Node destruction**: Automatic via `unique_ptr` when removed or program exits
- **Edge creation**: Via `AtomicGraph::add_edge()`
- **Edge removal**: Automatic when nodes deleted; manual via `remove_edge()`

## Summary

| Data Type | Storage Location | Structure | Size |
|-----------|------------------|-----------|------|
| Nodes | `AtomicGraph::nodes_` | `unordered_map<NodeID, unique_ptr<Node>>` | ~1MB for 30K nodes |
| Edges | `AtomicGraph::edges_` | `unordered_map<NodeID, map<NodeID, Weight>>` | ~30MB for 30K nodes |
| Node Data | Heap arrays (`payload_`) | `uint8_t*` | Variable (1-768B per node) |

All storage is **in-memory**, **thread-safe**, and **temporary** (lost on program exit).

