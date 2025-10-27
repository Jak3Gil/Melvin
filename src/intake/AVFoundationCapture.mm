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

- (BOOL)start {
    // Request camera permission
    AVCaptureDeviceDiscoverySession *discovery = [AVCaptureDeviceDiscoverySession
        discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
        mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionBack];
    
    if (discovery.devices.count == 0) {
        NSLog(@"No camera available");
        return NO;
    }
    
    AVCaptureDevice *device = discovery.devices.firstObject;
    
    NSError *error = nil;
    AVCaptureDeviceInput *input = [[AVCaptureDeviceInput alloc] initWithDevice:device error:&error];
    if (error) {
        NSLog(@"Error creating input: %@", error);
        return NO;
    }
    
    if ([self.session canAddInput:input]) {
        [self.session addInput:input];
    }
    
    AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
    output.videoSettings = @{(NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_24RGB)};
    output.alwaysDiscardsLateVideoFrames = YES;
    [output setSampleBufferDelegate:self queue:capture_queue_];
    
    if ([self.session canAddOutput:output]) {
        [self.session addOutput:output];
    }
    
    [self.session startRunning];
    return YES;
}

- (void)stop {
    [self.session stopRunning];
    [self.session removeAllInputs];
    [self.session removeAllOutputs];
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

- (BOOL)start {
    NSError *error = nil;
    if (![self.engine startAndReturnError:&error]) {
        NSLog(@"Error starting audio: %@", error);
        return NO;
    }
    
    // Install tap on input node (20ms @ 16kHz = 640B)
    AVAudioFormat *format = [input_node_ outputFormatForBus:0];
    
    [input_node_ installTapOnBus:0 bufferSize:320 
                         format:format block:^(AVAudioPCMBuffer *buffer, AVAudioTime *when) {
        AVAudioFrameCount frameCount = buffer.frameLength;
        int16_t* data = (int16_t*)buffer.int16ChannelData[0];
        
        if (audio_size_ == 0) {
            audio_size_ = 640; // 20ms @ 16kHz
            latest_audio_ = (int16_t*)malloc(audio_size_ * sizeof(int16_t));
        }
        
        size_t copySize = (frameCount < audio_size_ / 2) ? frameCount : audio_size_ / 2;
        memcpy(latest_audio_, data, copySize * sizeof(int16_t));
        audio_ready_.store(true);
    }];
    
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

