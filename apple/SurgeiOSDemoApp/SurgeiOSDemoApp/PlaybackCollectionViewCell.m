//
//  PlaybackCollectionViewCell.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "PlaybackCollectionViewCell.h"
#import "RtspAddress.h"

NSString *const StreamRemovalRequestNotfication = @"StreamRemovalRequestNotfication";

@interface PlaybackCollectionViewCell () <SurgeRtspPlayerDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (weak, nonatomic) IBOutlet UIImageView *playbackView;
@property (weak, nonatomic) IBOutlet UILabel *urlLabel;
@property (weak, nonatomic) IBOutlet UIButton *playPauseButton;
@property (weak, nonatomic) IBOutlet UIButton *closeButton;
@property (nonatomic, getter=isPlaying) BOOL playing;
@end

@implementation PlaybackCollectionViewCell

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    
  }
  return self;
}

- (void)awakeFromNib {
  [super awakeFromNib];
  self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
  self.rtspPlayer.delegate = self;
  self.rtspPlayer.playerView = self.playbackView;
  self.urlLabel.text = @"";
}

- (void)prepareForReuse {
  [super prepareForReuse];
  self.rtspPlayer.delegate = nil;
  self.rtspPlayer = nil;
  self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
  self.rtspPlayer.delegate = self;
}

#pragma mark - Properties

- (void)setRtspAddress:(RtspAddress *)rtspAddress {
  _rtspAddress = rtspAddress;
  self.urlLabel.text = rtspAddress.address;
  [self.activityIndicator startAnimating];
  [self.playPauseButton setImage:nil forState:UIControlStateNormal];
  
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
    NSURL *url = [NSURL URLWithString:rtspAddress.address];
    if (rtspAddress.username && rtspAddress.password) {
      [self.rtspPlayer initiatePlaybackOf:url withUsername:rtspAddress.username andPassword:rtspAddress.password];
    }
    else {
      [self.rtspPlayer initiatePlaybackOf:url];
    }
  });
}

#pragma mark - Interface

- (void)setupInterfaceForPlaying:(BOOL)playing {
  self.playing = playing;
  
  UIImage *image = [UIImage imageNamed:playing ? @"play-icon" : @"pause-icon"];
  [self.playPauseButton setImage:image forState:UIControlStateNormal];
  
  self.playPauseButton.alpha = 1;
  [UIView animateWithDuration:1.5
                   animations:^{
                     self.playPauseButton.alpha = 0;
                   } completion:^(BOOL finished) {
                     [self.playPauseButton setImage:nil forState:UIControlStateNormal];
                   }];
  
  if (playing) {
    [self.activityIndicator stopAnimating];
  }
}

#pragma mark - Actions

- (IBAction)playPauseButtonAction:(id)sender {
  if (self.isPlaying) {
    [self.rtspPlayer pause];
  }
  else {
    [self.rtspPlayer play];
  }
}

- (IBAction)closeButtonAction:(id)sender {
  self.rtspPlayer.delegate = nil;
  __weak typeof(self.rtspPlayer) weakRtspPlayer = self.rtspPlayer;
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    [weakRtspPlayer stop];
  });
  [[NSNotificationCenter defaultCenter] postNotificationName:StreamRemovalRequestNotfication
                                                      object:@(self.index)];
}

#pragma mark - SurgeRtspPlayerDelegate

/**
 * Called when the player begins or resumes playback of a stream.
 */
- (void)rtspPlayerDidBeginPlayback:(SurgeRtspPlayer *)player {
  NSLog(@"Did begin playback...");
  [self setupInterfaceForPlaying:YES];
}

- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player {
  UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Oops"
                                                                 message:@"Failed to start stream"
                                                          preferredStyle:UIAlertControllerStyleAlert];
  [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
//    [self.navigationController popToRootViewControllerAnimated:YES];
  }]];
//  [self presentViewController:alert animated:YES completion:nil];
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
