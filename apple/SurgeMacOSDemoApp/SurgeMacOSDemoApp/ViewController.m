// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <SurgeMacOS/SurgeMacOS.h>

#import "ViewController.h"
#import "Constants.h"

@interface ViewController () <SurgeRtspPlayerDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@end

@implementation ViewController

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:RtspAddressSelectionNotification object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
    self.rtspPlayer.delegate = self;
    self.rtspPlayer.playerView = self.playbackView;
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(loadRtspStreamFromNotification:)
                                                 name:RtspAddressSelectionNotification
                                               object:nil];
}

#pragma mark - Actions

- (void)loadRtspStreamFromNotification:(NSNotification *)notification {
    NSString *rtspAddress = notification.object;
    if (![rtspAddress isKindOfClass:[NSString class]]) {
        return;
    }
    [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:rtspAddress]];
}

@end
