// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import <SurgeiOS/SurgeiOS.h>

#import "ViewController.h"

@interface ViewController () <SurgeRtspPlayerDelegate, UITextFieldDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@end

@implementation ViewController

- (void)viewDidLoad {
    _rtspPlayer = [[SurgeRtspPlayer alloc] init];
    [_rtspPlayer setDelegate:self];
    [self embedPlayerView:[_rtspPlayer playerView]];
    [_activityIndicator startAnimating];
    [_activityIndicator setHidden:YES];
    [_streamUrlField setDelegate:self];
}

- (void)viewDidAppear:(BOOL)animated {
    //[self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:@"rtsp://192.168.1.54:8554/test"]];
}

#pragma mark - View Customization

- (void)embedPlayerView:(UIView*)playerView {
    
    [playerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [_playbackViewHolder addSubview:playerView];
    [_playbackViewHolder bringSubviewToFront:_activityIndicator];
    
    NSDictionary *keyedViews = [NSDictionary dictionaryWithObjectsAndKeys:playerView, @"playerView", nil];
    NSArray *constraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[playerView]-0-|"
                                                                   options:NSLayoutFormatAlignAllLeft
                                                                   metrics:nil
                                                                     views:keyedViews];
    [NSLayoutConstraint activateConstraints:constraints];
    
    
    constraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[playerView]-0-|"
                                                          options:NSLayoutFormatAlignAllTop
                                                          metrics:nil
                                                            views:keyedViews];
    [NSLayoutConstraint activateConstraints:constraints];
    
    
    
}

#pragma mark - Playback Controls

- (IBAction)tappedPauseButton:(id)sender {
    [_rtspPlayer pause];
}

- (IBAction)tappedPlayButton:(id)sender {
    [_rtspPlayer play];
}

#pragma mark - SurgeRtspPlayerDelegate

/**
 * Called when the player begins or resumes playback of a stream.
 */
- (void)rtspPlayerDidBeginPlayback {
    NSLog(@"Did begin playback...");
}

/**
 * Called when the player enters the buffering state.
 */
- (void)rtspPlayerDidBeginBuffering {
    NSLog(@"Did begin buffering...");
}

/**
 * Called when the player exits the buffering state.
 */
- (void)rtspPlayerDidStopBuffering {
    NSLog(@"Did stop buffering...");
}

/**
 * Called when the player times out.
 */
- (void)rtspPlayerDidTimeout {
    
}

/**
 * Guaranteed to be call at most once per second with the current player frame rate.
 */
- (void)rtspPlayerDidObservePlaybackFrameRate:(NSUInteger)frameRate {
    NSLog(@"Frame rate: %d", frameRate);
}

#pragma mark - UITextFieldDelegate

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    NSLog(@"%@", string);
    return YES;
}


@end
