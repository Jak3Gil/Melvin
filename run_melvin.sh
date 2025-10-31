#!/bin/bash

# Run Melvin with real camera/mic data and live visualization
cd /Users/jakegilbert/Desktop/MELVIN/Melvin

echo "=== MELVIN v1.0 - Real-Time Cognitive Architecture ==="
echo "Starting with camera + microphone capture..."
echo ""

# Run in foreground to see output
./build/melvin 2>&1

