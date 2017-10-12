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
@property (weak, nonatomic) IBOutlet UIImageView *playPauseIndicator;
@property (weak, nonatomic) IBOutlet UIButton *closeButton;
@end

@implementation PlaybackCollectionViewCell

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapped:)];
    [self.contentView addGestureRecognizer:tapGesture];
  }
  return self;
}

- (void)prepareForReuse {
  [super prepareForReuse];
}

- (void)dealloc {
  if (self.stream && self.stream.isPlaying) {
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
  self.playPauseIndicator.image =  [UIImage imageNamed:playing ? @"play-icon" : @"pause-icon"];
  self.playPauseIndicator.alpha = 1;
  [UIView animateWithDuration:1.5
                   animations:^{
                     self.playPauseIndicator.alpha = 0;
                     self.playPauseIndicator.transform = CGAffineTransformMakeScale(5, 5);
                   } completion:^(BOOL finished) {
                     self.playPauseIndicator.image = nil;
                     self.playPauseIndicator.transform = CGAffineTransformIdentity;
                   }];
  if (playing) {
    [self.activityIndicator stopAnimating];
  }
  else {
    [self.activityIndicator startAnimating];
  }
}

#pragma mark - Actions

- (void)tapped:(UITapGestureRecognizer *)sender {
  if (sender.state != UIGestureRecognizerStateEnded) return;
  if (self.stream.isPlaying) {
    [self.stream pause];
  }
  else {
    [self.stream play];
  }
}

- (IBAction)closeButtonAction:(id)sender {
  if (self.stream) {
    [self.stream stop];
    [self.stream removeObserver:self forKeyPath:@"state"];
  }
  [[NSNotificationCenter defaultCenter] postNotificationName:StreamRemovalRequestNotfication
                                                      object:@(self.index)];
}


@end
