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
#import "NSArray+RtspAddressStorage.h"

@interface ViewController ()
<SurgeRtspPlayerDelegate, UITableViewDelegate, UITableViewDataSource, UISearchDisplayDelegate, UISearchBarDelegate>
@property (nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@property (nonatomic, copy) NSArray <NSString *> *allStoredAddresses;
@property (nonatomic, copy) NSArray <NSString *> *storedAddressSearchResults;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    _rtspPlayer = [[SurgeRtspPlayer alloc] init];
    [_rtspPlayer setDelegate:self];
    [self embedPlayerView:[_rtspPlayer playerView]];
    [_activityIndicator startAnimating];
    [_activityIndicator setHidden:YES];
    
    self.allStoredAddresses = [NSArray<NSString *> storedRtspAddresses];
    self.storedAddressSearchResults = @[];
    
    [self.searchDisplayController.searchResultsTableView registerClass:[UITableViewCell class]
                                                forCellReuseIdentifier:@"SearchResultTableViewCell"];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}

#pragma mark - View Customization

- (void)embedPlayerView:(UIView*)playerView {
    
    [playerView setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [_playbackViewHolder addSubview:playerView];
    [_playbackViewHolder bringSubviewToFront:_activityIndicator];
    
    NSDictionary *keyedViews = @{@"playerView" : playerView};
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
    NSLog(@"Frame rate: %@", @(frameRate));
}

#pragma mark - UISearchDisplayDelegate

- (BOOL)searchDisplayController:(UISearchDisplayController *)controller shouldReloadTableForSearchString:(nullable NSString *)searchString {
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"self CONTAINS[cd] %@", searchString];
    self.storedAddressSearchResults = [self.allStoredAddresses filteredArrayUsingPredicate:predicate];
    return YES;
}

#pragma mark - UISearchBarDelegate

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    // save user input and load stream
    self.allStoredAddresses = [self.allStoredAddresses arrayByAddingObject:searchBar.text];
    [self.allStoredAddresses saveAsStoredRtspAddresses];
    [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:searchBar.text]];
    [self.searchDisplayController setActive:NO animated:YES];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if ([tableView isEqual:self.searchDisplayController.searchResultsTableView]) {
        return self.storedAddressSearchResults.count;
    }
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    if ([tableView isEqual:self.searchDisplayController.searchResultsTableView]) {
        UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"SearchResultTableViewCell"];
        cell.textLabel.text = self.storedAddressSearchResults[indexPath.row];
        return cell;
    }
    return nil;
}

#pragma mark - UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if ([tableView isEqual:self.searchDisplayController.searchResultsTableView]) {
        [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:self.storedAddressSearchResults[indexPath.row]]];
        [self.searchDisplayController setActive:NO animated:YES];
    }
}

@end
