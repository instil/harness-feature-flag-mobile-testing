//
//  ViewController.m
//  SurgeH264Test
//
//  Created by Philip Herron on 07/03/2016.
//  Copyright © 2016 Philip Herron. All rights reserved.
//

#import "ViewController.h"
#import "RtspClientWrapper.h"
#import "H264DecoderWrapper.h"

@interface ViewController () <RtspClientDelegate, H264DecoderDelegate>
@property (strong, nonatomic) IBOutlet UIImageView *videoView;

@property (nonatomic, strong) RtspClientWrapper *client;
@property (nonatomic, strong) H264DecoderWrapper *decoder;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    self.decoder = [[H264DecoderWrapper alloc] initWithDelegate:self];
    [self.decoder startDecoder];
    
    self.client = [[RtspClientWrapper alloc] initWithUrl:@"rtsp://192.168.1.93:8554/test"
                                             andDelegate:self];
    [self.client playStream];
}


#pragma mark - Rtsp Delegate

- (void)Timeout {
    NSLog(@"TIMEOUT");
}

- (void)Payload:(const unsigned char*)payload withSize:(size_t)length {
    NSLog(@"PAYLOAD");
    [self.decoder decodeFrameBuffer:payload ofSize:length withFrameDuration:0 andPresentationTime:0];
}

#pragma mark - H264 Delegate

- (void)decodedFrame:(CGImageRef)imageBuffer withTimeStamp:(double)timestamp {
    dispatch_sync(dispatch_get_main_queue(), ^{
        NSLog(@"DECODED FRAME");
        self.videoView.image = [[UIImage alloc] initWithCGImage:imageBuffer];
    });
}

- (void)decoderIsBuffering {
    
}

- (void)decoderStoppedBuffering {
    
}

- (void)framesPerSecond:(int)fps {
    NSLog(@"FPS: %i", fps);
}

@end
