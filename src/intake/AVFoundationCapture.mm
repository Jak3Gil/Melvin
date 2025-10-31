#import "AVFoundationCapture.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudio.h>
#import <atomic>

@implementation MelvinCameraCapture {
    uint8_t* latest_frame_;
    size_t frame_size_;
    dispatch_queue_t capture_queue_;
    std::atomic<bool> frame_ready_;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        latest_frame_ = nullptr;
        frame_size_ = 0;
        frame_ready_ = false;
        capture_queue_ = dispatch_queue_create("com.melvin.camera", DISPATCH_QUEUE_SERIAL);
        self.session = [[AVCaptureSession alloc] init];
    }
    return self;
}

+ (BOOL)checkCameraPermission {
    AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    
    if (status == AVAuthorizationStatusNotDetermined) {
        NSLog(@"⚠️  Camera permission not determined. Requesting...");
        return NO;
    }
    
    if (status == AVAuthorizationStatusDenied || status == AVAuthorizationStatusRestricted) {
        NSLog(@"❌ Camera permission DENIED or RESTRICTED");
        NSLog(@"   Please grant permission in: System Settings → Privacy & Security → Camera");
        NSLog(@"   Or reset with: tccutil reset Camera");
        return NO;
    }
    
    return YES;
}

- (BOOL)start {
    NSLog(@"🎥 === Camera Intake Starting ===");
    
    // Check permission
    if (![MelvinCameraCapture checkCameraPermission]) {
        return NO;
    }
    
    // Request camera permission if not determined
    __block BOOL permissionGranted = NO;
    [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo 
                               completionHandler:^(BOOL granted) {
        permissionGranted = granted;
        NSLog(@"Camera permission request: %@", granted ? @"✅ GRANTED" : @"❌ DENIED");
    }];
    
    // Give permission request a moment
    for (int i = 0; i < 10 && !permissionGranted; i++) {
        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
    
    if (!permissionGranted) {
        NSLog(@"❌ Camera permission denied by user");
        return NO;
    }
    
    // Discover devices
    AVCaptureDeviceDiscoverySession *discovery = [AVCaptureDeviceDiscoverySession
        discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
        mediaType:AVMediaTypeVideo 
        position:AVCaptureDevicePositionBack];
    
    NSLog(@"Found %lu camera device(s)", (unsigned long)discovery.devices.count);
    
    if (discovery.devices.count == 0) {
        NSLog(@"❌ No camera available");
        return NO;
    }
    
    // Select device
    AVCaptureDevice *device = discovery.devices.firstObject;
    NSLog(@"Using device: %@", device.localizedName);
    NSLog(@"Device ID: %@", device.uniqueID);
    NSLog(@"Backend: AVFoundation (macOS)");
    
    // Configure device
    NSError *configError = nil;
    if ([device lockForConfiguration:&configError]) {
        if ([device isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure]) {
            device.exposureMode = AVCaptureExposureModeContinuousAutoExposure;
        }
        if ([device isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus]) {
            device.focusMode = AVCaptureFocusModeContinuousAutoFocus;
        }
        [device unlockForConfiguration];
    }
    
    // Create input
    NSError *error = nil;
    AVCaptureDeviceInput *input = [[AVCaptureDeviceInput alloc] initWithDevice:device error:&error];
    if (error) {
        NSLog(@"❌ Error creating camera input: %@", error.localizedDescription);
        return NO;
    }
    
    // Configure session - use HIGH quality for full resolution capture
    [self.session beginConfiguration];
    self.session.sessionPreset = AVCaptureSessionPresetHigh; // High quality for max resolution
    
    if ([self.session canAddInput:input]) {
        [self.session addInput:input];
        NSLog(@"✅ Camera input added");
    } else {
        NSLog(@"❌ Cannot add camera input");
        [self.session commitConfiguration];
        return NO;
    }
    
    // Configure output
    AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
    output.videoSettings = @{(NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_24RGB)};
    output.alwaysDiscardsLateVideoFrames = YES;
    [output setSampleBufferDelegate:self queue:capture_queue_];
    
    if ([self.session canAddOutput:output]) {
        [self.session addOutput:output];
        NSLog(@"✅ Camera output added");
        
        // Log format info
        CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions(device.activeFormat.formatDescription);
        CMTime frameDuration = device.activeVideoMinFrameDuration;
        Float64 fps = 30.0;
        if (frameDuration.value > 0) {
            fps = CMTimeGetSeconds(frameDuration) > 0 ? 1.0 / CMTimeGetSeconds(frameDuration) : 30.0;
        }
        NSLog(@"Capture format: %lux%lu @ %.2f fps", (unsigned long)dims.width, (unsigned long)dims.height, fps);
        NSLog(@"Target frame size: 16x16 RGB (768 bytes)");
    } else {
        NSLog(@"❌ Cannot add camera output");
        [self.session commitConfiguration];
        return NO;
    }
    
    [self.session commitConfiguration];
    
    // Start session
    [self.session startRunning];
    NSLog(@"✅ Camera session started successfully");
    NSLog(@"   Camera LED should be ON (green indicator)");
    
    return YES;
}

- (void)stop {
    [self.session stopRunning];
    for (AVCaptureInput *input in self.session.inputs) {
        [self.session removeInput:input];
    }
    for (AVCaptureOutput *output in self.session.outputs) {
        [self.session removeOutput:output];
    }
}

- (uint8_t*)getLatestFrame:(size_t)frameSize {
    // Return pointer to latest captured frame
    if (latest_frame_ && frame_ready_.load()) {
        return latest_frame_;
    }
    return nullptr;
}

- (void)captureOutput:(AVCaptureOutput *)output 
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
           fromConnection:(AVCaptureConnection *)connection {
    
    // Extract frame data (16x16 RGB = 768 bytes)
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    
    // Resize to 16x16 and copy
    if (frame_size_ == 0) {
        frame_size_ = 768; // 16x16x3
        latest_frame_ = (uint8_t*)malloc(frame_size_);
    }
    
    // Simple downscale (nearest neighbor)
    for (size_t y = 0; y < 16; y++) {
        for (size_t x = 0; x < 16; x++) {
            size_t src_y = y * height / 16;
            size_t src_x = x * width / 16;
            uint8_t* pixel = (uint8_t*)baseAddress + src_y * bytesPerRow + src_x * 3;
            
            latest_frame_[y * 48 + x * 3 + 0] = pixel[0]; // R
            latest_frame_[y * 48 + x * 3 + 1] = pixel[1]; // G
            latest_frame_[y * 48 + x * 3 + 2] = pixel[2]; // B
        }
    }
    
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    
    // Mark frame as ready
    frame_ready_.store(true);
}

@end

@implementation MelvinAudioCapture {
    int16_t* latest_audio_;
    size_t audio_size_;
    AVAudioNode *input_node_;
    std::atomic<bool> audio_ready_;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        latest_audio_ = nullptr;
        audio_size_ = 0;
        audio_ready_ = false;
        self.engine = [[AVAudioEngine alloc] init];
        input_node_ = self.engine.inputNode;
    }
    return self;
}

+ (BOOL)checkMicrophonePermission {
    // On macOS, AVAudioEngine doesn't require explicit permission check
    // The system will prompt if needed
    NSLog(@"⚠️  Checking microphone permission...");
    return YES;
}

- (BOOL)start {
    NSLog(@"🎤 === Audio Intake Starting ===");
    
    // Check permission
    if (![MelvinAudioCapture checkMicrophonePermission]) {
        return NO;
    }
    
    // On macOS, AVAudioEngine handles permissions automatically
    // The system will prompt if needed
    NSLog(@"⚠️  macOS will prompt for microphone permission if needed");
    
    // Configure engine
    NSError *error = nil;
    
    // Get format info
    AVAudioFormat *format = [input_node_ outputFormatForBus:0];
    double sampleRate = format.sampleRate;
    AVAudioChannelCount channels = format.channelCount;
    
    NSLog(@"Audio format:");
    NSLog(@"  Sample rate: %.0f Hz", sampleRate);
    NSLog(@"  Channels: %u", (unsigned int)channels);
    NSLog(@"  Backend: CoreAudio (macOS)");
    
    // Start engine
    if (![self.engine startAndReturnError:&error]) {
        NSLog(@"❌ Error starting audio engine: %@", error.localizedDescription);
        return NO;
    }
    
    NSLog(@"✅ Audio engine started");
    
    // Install tap on input node (20ms @ 16kHz = 640B)
    NSUInteger frameLength = (NSUInteger)(sampleRate * 0.02); // 20ms
    [input_node_ installTapOnBus:0 bufferSize:(AUAudioFrameCount)frameLength 
                         format:format block:^(AVAudioPCMBuffer *buffer, AVAudioTime *when) {
        AVAudioFrameCount frameCount = buffer.frameLength;
        
        // Convert to int16
        if (buffer.int16ChannelData && buffer.int16ChannelData[0]) {
            int16_t* data = (int16_t*)buffer.int16ChannelData[0];
            
            if (self->audio_size_ == 0) {
                self->audio_size_ = 640; // 20ms @ 16kHz
                self->latest_audio_ = (int16_t*)malloc(self->audio_size_ * sizeof(int16_t));
            }
            
            size_t copySize = (frameCount < self->audio_size_ / 2) ? frameCount : self->audio_size_ / 2;
            memcpy(self->latest_audio_, data, copySize * sizeof(int16_t));
            self->audio_ready_.store(true);
        }
    }];
    
    NSLog(@"✅ Audio tap installed");
    NSLog(@"Target buffer: 20ms @ 16kHz mono (640 bytes)");
    
    return YES;
}

- (void)stop {
    [input_node_ removeTapOnBus:0];
    [self.engine stop];
}

- (int16_t*)getLatestAudio:(size_t)audioSize {
    if (latest_audio_ && audio_ready_.load()) {
        return latest_audio_;
    }
    return nullptr;
}

@end

