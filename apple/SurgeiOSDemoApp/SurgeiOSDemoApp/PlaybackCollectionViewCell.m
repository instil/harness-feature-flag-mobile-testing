//
//  PlaybackCollectionViewCell.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "PlaybackCollectionViewCell.h"

@interface PlaybackCollectionViewCell () <SurgeRtspPlayerDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;
@property (weak, nonatomic) IBOutlet UIImageView *playbackView;
@property (weak, nonatomic) IBOutlet UILabel *urlLabel;
@end

@implementation PlaybackCollectionViewCell

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    self.rtspPlayer = [[SurgeRtspPlayer alloc] init];
    self.rtspPlayer.delegate = self;
    self.rtspPlayer.playerView = self.playbackView;
    self.urlLabel.text = @"";
  }
  return self;
}

- (void)prepareForReuse {
  [super prepareForReuse];
  self.rtspPlayer.delegate = nil;
  self.rtspPlayer = nil;
}

#pragma mark - Properties

- (void)setPlaybackUrlString:(NSString *)playbackUrlString {
  _playbackUrlString = playbackUrlString;
  self.urlLabel.text = playbackUrlString;
}

#pragma mark - Interface

- (void)setupInterfaceForPlaying:(BOOL)playing {
//  UIBarButtonSystemItem systemItem = playing ? UIBarButtonSystemItemPause : UIBarButtonSystemItemPlay;
//  SEL action = playing ? @selector(tappedPauseButton:) : @selector(tappedPlayButton:);
//  UIBarButtonItem *newItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:systemItem
//                                                                           target:self
//                                                                           action:action];
//  NSMutableArray *m_toolbarItems = self.playbackControlsToolbar.items.mutableCopy;
//  [m_toolbarItems replaceObjectAtIndex:1 withObject:newItem];
//  self.playbackControlsToolbar.items = m_toolbarItems;
  
  if (playing) {
    [self.activityIndicator stopAnimating];
  }
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
