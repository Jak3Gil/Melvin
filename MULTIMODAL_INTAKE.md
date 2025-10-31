# Multimodal Hardware Intake

## ✅ Implemented

### Components Created

**`src/intake/HardwareCapture.h/cpp`** - Captures from camera and microphone
- Simulated hardware capture (generates mock data)
- Thread-safe frame and audio buffer access
- Ready for real AVFoundation integration

**`src/intake/MultimodalIntake.h/cpp`** - Continuous intake loop
- Captures vision frames and audio chunks in parallel
- Feeds data into IntakeManager → AtomicGraph
- Reports progress every second

### How It Works

1. **Vision**: 16×16 pixel RGB frames (768 bytes each)
2. **Audio**: 20ms chunks @ 16kHz (640 bytes each)
3. **Both captured simultaneously** in separate threads
4. **Fed into the same graph** as the unified cognitive architecture

### Integration

Added to main loop:
```cpp
auto multimodal_intake = std::make_unique<MultimodalIntake>(intake_manager.get(), graph.get());
multimodal_intake->start();  // Begins continuous capture
```

### Current Status

✅ **Builds successfully**
✅ **Integrated into main loop**
⚠️ **Currently generates simulated data** (not real hardware yet)

### To Enable Real Hardware

Need to implement actual AVFoundation capture on macOS:
1. Request camera/microphone permissions
2. Set up AVCaptureSession for camera
3. Set up AVAudioEngine for microphone
4. Convert captured frames to 16×16 RGB chunks
5. Convert audio samples to 20ms @ 16kHz chunks

### Testing

Run Melvin now and you'll see:
```
Starting hardware capture (camera + microphone)...
Captured 50 vision frames, 50 audio chunks
```

The simulated data feeds through the same graph, so you can test how Melvin handles concurrent multimodal input even without real hardware.

