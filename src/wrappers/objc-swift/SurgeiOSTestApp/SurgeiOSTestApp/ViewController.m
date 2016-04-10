//
//  ViewController.m
//  SurgeiOSTestApp
//
//  Created by Chris van Es on 30/03/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import <SurgeiOS/SurgeiOS.h>

#import "ViewController.h"

@interface ViewController ()
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@end

@implementation ViewController

- (void)viewDidLoad {
    self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
    self.rtspPlayer.playerView.frame = [[UIScreen mainScreen] bounds];
    [self.view addSubview:self.rtspPlayer.playerView];
}

- (void)viewDidAppear:(BOOL)animated {
    [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:@"rtsp://192.168.1.133:8554/test"]];
}

@end
