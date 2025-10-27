#!/usr/bin/env python3
"""Create synthetic text data for Melvin testing."""

import os

os.makedirs("data", exist_ok=True)

# Create a larger synthetic dataset
text_content = """

INTRODUCTION TO MELVIN

Melvin is a unified cognitive architecture where all sensory data flows through a single self-organizing graph structure. Nodes represent chunks of sensory or internal data including vision audio text and motor information.

The system operates on principles of co-activation temporal locality and energy-based reasoning. When nodes appear together frequently they form strong connections. These connections create pathways for information flow that drive attention movement and understanding.

KEY FEATURES

First the intake layer processes raw data by chunking it into fixed-size payloads. Vision uses 768 bytes for 16 by 16 RGB pixel blocks. Audio captures 640 bytes representing 20 milliseconds at 16 kilohertz sampling. Text creates one byte per character. Motor feedback includes position velocity torque and electrical state.

Second connection formation happens through temporal neighborhood. New nodes connect to previous and subsequent nodes creating chains of co-occurrence. Over time these connections strengthen based on frequency and co-activation patterns.

Third generalization emerges through leap nodes and leap connections. When three nodes form a fully connected triangle with high weight sum they consolidate into a single abstract representation. Similarly when two nodes connect to the same target with high average weight they create shortcut connections.

Fourth reasoning uses energy field dynamics. Each active node has an energy value that decays over time but is replenished by neighbor activation. Reasoning continues until the system reaches a stable coherent state where internal connections dominate external ones.

Fifth outputs emerge from active nodes. Motor nodes drive physical movement through CAN bus communication. Vision nodes guide attention to specific spatial locations. Audio nodes trigger sound or speech. Text nodes produce written output.

Sixth evolution adapts parameters over time. Fitness combines prediction accuracy coherence stability and computational efficiency. When fitness improves parameters stabilize. When fitness drops parameters mutate slightly to explore new configurations.

Seventh pruning removes unused connections. Nodes and edges decay based on frequency divided by age. Low scoring elements get deleted keeping the graph efficient and focused on recent patterns.

FEEDBACK LOOP

Every output feeds back into intake creating a continuous perception action learning cycle. Melvin's own responses become part of its next sensory state allowing self-observation reinforcement and recursive understanding.

APPLICATIONS

This architecture enables embodied AI that learns from experience adapts to context and develops understanding through simple association rather than explicit programming. The same graph processes vision audio text and motor data without type tags creating a truly unified cognitive space.

""" * 5  # Repeat 5 times for more data

with open("data/melvin_documentation.txt", "w", encoding="utf-8") as f:
    f.write(text_content)

print("Created data/melvin_documentation.txt")
print(f"Total characters: {len(text_content)}")

