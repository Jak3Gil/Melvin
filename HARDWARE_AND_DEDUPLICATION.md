# Hardware Capture & Deduplication

## ✅ Added Features

### 1. Deduplication by Payload

**New Method in AtomicGraph:**
```cpp
NodeID find_node_with_payload(const void* payload, size_t payload_size) const;
```

**How it works:**
- Searches all nodes for matching payloads using `memcmp()`
- Returns existing node ID if found
- Returns 0 if not found (payload is new)

**Benefits:**
- No duplicate nodes for same data
- Efficient memory usage
- Preserves connection history to repeated content

### 2. Real Camera/Microphone Access (Why Not Working Yet)

**Current Limitation:**
The code is C++ and standard AVFoundation requires Objective-C/C++ bridges. To use real hardware we need:

1. **Objective-C++ wrapper** for AVFoundation
   - Bridge between C++ and Objective-C
   - Request camera/mic permissions
   - Handle AVFoundation callbacks

2. **Framework linking**:
   ```cmake
   target_link_libraries(melvin PRIVATE 
       "-framework AVFoundation"
       "-framework CoreAudio"
   )
   ```

3. **Info.plist permissions**:
   - `NSCameraUsageDescription`
   - `NSMicrophoneUsageDescription`

**Why it's using simulated data:**
- Simpler to test the architecture first
- AVFoundation integration requires Objective-C++ mixing
- Real hardware capture is the next step

### 3. Current Behavior

**What happens now:**
1. Tries to read from `latest_vision_` buffer (when real hardware available)
2. Falls back to simulated data (random pixels/audio)
3. Checks for duplicate payloads before adding node
4. Only creates new node if data is truly new

**To enable real hardware:**
1. Create `src/intake/AVFoundationCapture.mm` (Objective-C++)
2. Implement camera/mic capture
3. Link frameworks in CMakeLists.txt
4. Add Info.plist permissions

The architecture is ready - it just needs the hardware bridge layer!

