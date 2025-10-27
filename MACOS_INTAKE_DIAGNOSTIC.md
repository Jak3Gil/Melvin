# macOS Intake Diagnostic Report

## Current Implementation Status

### ✅ What's Already Built

The current Melvin implementation uses **AVFoundation directly** (not OpenCV + PortAudio):

**Architecture:**
```
HardwareCapture.mm (C++ wrapper)
    ↓
AVFoundationCapture.mm (Objective-C++ bridge)
    ↓
AVFoundation/CoreAudio (macOS native APIs)
```

**Files:**
- `src/intake/HardwareCapture.mm` - C++ wrapper with fallback simulation
- `src/intake/AVFoundationCapture.mm` - Objective-C++ implementation
- `src/intake/AVFoundationCapture.h` - Objective-C++ interface

**CMake Configuration:**
- ✅ Already links AVFoundation, CoreAudio, CoreVideo, CoreMedia
- ✅ Sets Objective-C++ compilation flags
- ✅ macOS-specific compilation enabled

---

## Current Issues to Address

### 1. **Camera Intake - Diagnostic & Improvements**

**Status**: Basic implementation exists but needs enhancement

**Issues:**
- No backend verification logging
- No permission request handling
- No frame dimension/FPS diagnostics
- No graceful fallback documentation

**Required Changes:**
```cpp
// In AVFoundationCapture.mm
- Add logging for device ID, backend name (always "AVFoundation")
- Log frame dimensions, FPS
- Add permission request flow
- Better error handling
```

### 2. **Audio Intake - Diagnostic & Improvements**

**Status**: Basic implementation exists but needs enhancement

**Issues:**
- No device enumeration logging
- No default input device selection logic
- No sample rate query
- No PortAudio-style error messages

**Required Changes:**
```cpp
// In AVFoundationCapture.mm
- Add AVAudioDevice enumeration
- Select "Built-in" or "MacBook" mic explicitly
- Query sample rate and max channels
- Add detailed error logging
```

### 3. **Permission Handling**

**Missing:**
- Startup message reminding user to grant permissions
- README section for macOS setup
- `tccutil` reset commands documented

**Required:**
- Add permission request flow in `AVFoundationCapture.mm`
- Add README.md section "### macOS Setup"
- Document `tccutil reset Camera` and `tccutil reset Microphone`

### 4. **Logging & Diagnostics**

**Missing:**
- Device list logging
- Backend name logging (for camera)
- Sample rate logging (for audio)
- Clear error messages for failures

---

## Implementation Plan

### Step 1: Enhance Camera Capture (`AVFoundationCapture.mm`)

**Add Logging:**
```objc
- (BOOL)start {
    NSLog(@"🎥 Camera Intake Starting...");
    
    // Log device discovery
    AVCaptureDeviceDiscoverySession *discovery = ...;
    NSLog(@"Found %lu camera devices", (unsigned long)discovery.devices.count);
    
    // Log selected device
    AVCaptureDevice *device = discovery.devices.firstObject;
    NSLog(@"Using device: %@", device.localizedName);
    NSLog(@"Backend: AVFoundation");
    
    // Log format
    NSLog(@"Frame format: 16x16 RGB (768 bytes)");
    
    // Start session
    [self.session startRunning];
    NSLog(@"✅ Camera session started");
    
    return YES;
}
```

**Add Permission Handling:**
```objc
+ (BOOL)checkCameraPermission {
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    
    if (status == AVAuthorizationStatusNotDetermined) {
        // Request permission
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo 
                                   completionHandler:^(BOOL granted) {
            NSLog(@"Camera permission: %@", granted ? @"GRANTED" : @"DENIED");
        }];
        return NO; // Not ready yet
    }
    
    if (status == AVAuthorizationStatusDenied) {
        NSLog(@"❌ Camera permission DENIED. Grant permission in System Settings → Privacy & Security → Camera");
        return NO;
    }
    
    return YES;
}
```

### Step 2: Enhance Audio Capture (`AVFoundationCapture.mm`)

**Add Device Enumeration:**
```objc
- (BOOL)start {
    NSLog(@"🎤 Audio Intake Starting...");
    
    // Enumerate all audio devices
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    NSArray<AVAudioSessionPortDescription *> *inputs = [session availableInputs];
    NSLog(@"Found %lu audio input devices", (unsigned long)inputs.count);
    
    for (AVAudioSessionPortDescription *input in inputs) {
        NSLog(@"  - %@ (%@)", input.portName, input.portType);
    }
    
    // Try to select built-in mic
    for (AVAudioSessionPortDescription *input in inputs) {
        if ([input.portType isEqualToString:AVAudioSessionPortBuiltInMic]) {
            NSLog(@"Using built-in microphone");
            break;
        }
    }
    
    // Log sample rate
    double sampleRate = session.sampleRate;
    NSLog(@"Sample rate: %.0f Hz", sampleRate);
    
    // Start engine
    NSError *error = nil;
    if (![self.engine startAndReturnError:&error]) {
        NSLog(@"❌ Error starting audio engine: %@", error);
        return NO;
    }
    
    NSLog(@"✅ Audio engine started");
    return YES;
}
```

### Step 3: Update CMakeLists.txt (Already Done ✅)

Current configuration is already correct:
```cmake
target_link_libraries(melvin PRIVATE 
    "-framework AVFoundation"
    "-framework CoreAudio"
    "-framework CoreVideo"
    "-framework CoreMedia"
    "-framework Foundation"
    "-framework AppKit"
)
```

### Step 4: Add Permission Request to Main

```cpp
// In main.cpp
void requestMacPermissions() {
#ifdef __APPLE__
    // Request camera permission
    AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if (device) {
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo 
                                   completionHandler:^(BOOL granted) {
            NSLog(@"Camera: %@", granted ? @"✅" : @"❌");
        }];
    }
    
    // Request microphone permission
    [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted) {
        NSLog(@"Microphone: %@", granted ? @"✅" : @"❌");
    }];
#endif
}
```

### Step 5: Add README Section

```markdown
### macOS Setup

**Permissions Required:**
- Camera access
- Microphone access

**Grant Permissions:**
1. Open System Settings → Privacy & Security
2. Grant "Camera" access to Terminal/Cursor/VS Code
3. Grant "Microphone" access to Terminal/Cursor/VS Code

**Reset Permissions (if needed):**
```bash
tccutil reset Camera
tccutil reset Microphone
```

**Verify:**
Run `./build/melvin` - camera LED should turn ON
```

---

## Files to Modify

1. ✅ `src/intake/AVFoundationCapture.mm` - Add logging, permissions, diagnostics
2. ✅ `src/main.cpp` - Add permission request at startup
3. ✅ `README.md` - Add macOS setup section

---

## Next Steps

1. Read `AVFoundationCapture.mm` in detail
2. Add diagnostic logging for camera
3. Add diagnostic logging for audio
4. Add permission request flow
5. Update README.md
6. Test and rebuild

