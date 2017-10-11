//
//  PlaybackCollectionViewCell.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "PlaybackCollectionViewCell.h"
#import "PlaybackStream.h"
#import "RtspAddress.h"

NSString *const StreamRemovalRequestNotfication = @"StreamRemovalRequestNotfication";

@interface PlaybackCollectionViewCell ()
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

- (void)prepareForReuse {
  [super prepareForReuse];
}

- (void)dealloc {
  
}

#pragma mark - Properties

- (void)setStream:(PlaybackStream *)stream {
  _stream = stream;
  stream.player.playerView = self.playbackView;
  self.urlLabel.text = stream.rtspAddress.address;
  if (stream.rtspAddress.name) {
    self.urlLabel.text = [self.urlLabel.text stringByAppendingFormat:@" (%@)", stream.rtspAddress.name];
  }
  if (stream.state != PlaybackStreamStatePlaying) {
    [self.activityIndicator startAnimating];
  }
  [self.playPauseButton setImage:nil forState:UIControlStateNormal];
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
    [self.stream pause];
  }
  else {
    [self.stream play];
  }
}

- (IBAction)closeButtonAction:(id)sender {
  [[NSNotificationCenter defaultCenter] postNotificationName:StreamRemovalRequestNotfication
                                                      object:@(self.index)];
}


@end
