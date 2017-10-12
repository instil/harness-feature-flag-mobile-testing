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
  if (self.stream) {
    [self.stream removeObserver:self forKeyPath:@"state"];
  }
}

#pragma mark - Properties

- (void)setStream:(PlaybackStream *)stream {
  if (_stream) {
    [_stream removeObserver:self forKeyPath:@"state"];
  }
  _stream = stream;
  stream.player.playerView = self.playbackView;
  self.urlLabel.text = stream.rtspAddress.address;
  if (stream.rtspAddress.name) {
    self.urlLabel.text = [self.urlLabel.text stringByAppendingFormat:@" (%@)", stream.rtspAddress.name];
  }
  [self setupInterfaceForPlaying:stream.isPlaying];
  [stream addObserver:self forKeyPath:@"state" options:NSKeyValueObservingOptionNew context:NULL];
}

#pragma mark - KVO

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
  if (![object isEqual:self.stream]) return;
  dispatch_async(dispatch_get_main_queue(), ^{
    [self setupInterfaceForPlaying:self.stream.isPlaying];
  });
}

#pragma mark - Interface

- (void)setupInterfaceForPlaying:(BOOL)playing {
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
  else {
    [self.activityIndicator startAnimating];
  }
}

#pragma mark - Actions

- (IBAction)playPauseButtonAction:(id)sender {
  if (self.stream.isPlaying) {
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
