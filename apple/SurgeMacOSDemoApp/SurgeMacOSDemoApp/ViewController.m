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
