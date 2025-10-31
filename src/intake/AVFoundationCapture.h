#pragma once

#ifdef __APPLE__

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface MelvinCameraCapture : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
+ (BOOL)checkCameraPermission;
- (BOOL)start;
- (void)stop;
- (uint8_t*)getLatestFrame:(size_t)frameSize;
@property (nonatomic, strong) AVCaptureSession *session;
@end

@interface MelvinAudioCapture : NSObject <AVCaptureAudioDataOutputSampleBufferDelegate>
+ (BOOL)checkMicrophonePermission;
- (BOOL)start;
- (void)stop;
- (int16_t*)getLatestAudio:(size_t)audioSize;
@property (nonatomic, strong) AVAudioEngine *engine;
@end

#endif

