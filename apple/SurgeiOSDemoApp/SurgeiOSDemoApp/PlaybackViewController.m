//
//  PlaybackViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "PlaybackViewController.h"
#import <SurgeiOS/SurgeiOS.h>

@interface PlaybackViewController () <SurgeRtspPlayerDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (weak, nonatomic) IBOutlet UIImageView *playbackView;
@property (weak, nonatomic) IBOutlet UILabel *urlLabel;
@property (weak, nonatomic) IBOutlet UIToolbar *playbackControlsToolbar;
@property (weak, nonatomic, readonly) UIBarButtonItem *playbackButton;
@end

@implementation PlaybackViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
    self.rtspPlayer.delegate = self;
    self.rtspPlayer.playerView = self.playbackView;
    self.urlLabel.text = @"";
    [self setupInterfaceForPlaying:NO];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    if (self.playbackUrlString) {
        [self.activityIndicator startAnimating];
        self.urlLabel.text = self.playbackUrlString;
        [self playUrl:self.playbackUrlString];
        self.playbackUrlString = nil;
    }
    else {
        self.playbackButton.enabled = NO;
    }
}

#pragma mark - Actions

- (void)loadRtspStreamFromNotification:(NSNotification *)notification {
    NSString *rtspAddress = notification.object;
    if (![rtspAddress isKindOfClass:[NSString class]]) {
        return;
    }
    [self playUrl:rtspAddress];
}

- (void)playUrl:(NSString *)urlString {
    [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:urlString] withUsername:@"admin" andPassword:@"admin"];
}

- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player {
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Oops"
                                                                   message:@"Failed to start stream"
                                                            preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        [self.navigationController popToRootViewControllerAnimated:YES];
    }]];
    [self presentViewController:alert animated:YES completion:nil];
}


#pragma mark - Playback Controls

- (IBAction)tappedPauseButton:(id)sender {
    [self.rtspPlayer pause];
}

- (IBAction)tappedPlayButton:(id)sender {
    [self.rtspPlayer play];
}

- (void)setupInterfaceForPlaying:(BOOL)playing {
    UIBarButtonSystemItem systemItem = playing ? UIBarButtonSystemItemPause : UIBarButtonSystemItemPlay;
    SEL action = playing ? @selector(tappedPauseButton:) : @selector(tappedPlayButton:);
    UIBarButtonItem *newItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:systemItem
                                                                             target:self
                                                                             action:action];
    NSMutableArray *m_toolbarItems = self.playbackControlsToolbar.items.mutableCopy;
    [m_toolbarItems replaceObjectAtIndex:1 withObject:newItem];
    self.playbackControlsToolbar.items = m_toolbarItems;
    
    if (playing) {
        [self.activityIndicator stopAnimating];
    }
}

- (UIBarButtonItem *)playbackButton {
    return self.playbackControlsToolbar.items.count >= 2 ? self.playbackControlsToolbar.items[1] : nil;
}

#pragma mark - SurgeRtspPlayerDelegate

/**
 * Called when the player begins or resumes playback of a stream.
 */
- (void)rtspPlayerDidBeginPlayback:(SurgeRtspPlayer *)player {
    NSLog(@"Did begin playback...");
    [self setupInterfaceForPlaying:YES];
}

/**
 * Called when the player stops or pauses playback of a stream.
 */
- (void)rtspPlayerDidStopPlayback:(SurgeRtspPlayer *)player {
    NSLog(@"Did stop playback...");
    [self setupInterfaceForPlaying:NO];
}

/**
 * Called when the player enters the buffering state.
 */
- (void)rtspPlayerDidBeginBuffering:(SurgeRtspPlayer *)player {
    NSLog(@"Did begin buffering...");
    [self.activityIndicator startAnimating];
    [self setupInterfaceForPlaying:NO];
}

/**
 * Called when the player exits the buffering state.
 */
- (void)rtspPlayerDidStopBuffering:(SurgeRtspPlayer *)player {
    NSLog(@"Did stop buffering...");
    [self setupInterfaceForPlaying:YES];
}

/**
 * Called when the player times out.
 */
- (void)rtspPlayerDidTimeout:(SurgeRtspPlayer *)player {
    
}

/**
 * Guaranteed to be call at most once per second with the current player frame rate.
 */
- (void)rtspPlayer:(SurgeRtspPlayer *)player didObservePlaybackFrameRate:(NSUInteger)frameRate {
    NSLog(@"Frame rate: %@", @(frameRate));
}


@end
