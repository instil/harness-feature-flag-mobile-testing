//
//  PlaybackViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "PlaybackViewController.h"
#import "AddressesTableViewController.h"
#import "PlaybackCollectionViewCell.h"
#import "RtspAddress.h"
#import "PlaybackManager.h"

@interface PlaybackViewController () <UICollectionViewDataSource, UICollectionViewDelegate>
@property (nonatomic, weak) IBOutlet UICollectionView *collectionView;
@property (nonatomic, strong) PlaybackManager *playbackManager;
@end

@implementation PlaybackViewController

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (self) {
        
        self.playbackManager = [PlaybackManager new];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(reloadStreams)
                                                     name:PlaybackStreamsDidChangeNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(removeRtspStreamFromNotification:)
                                                     name:StreamRemovalRequestNotfication
                                                   object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:PlaybackStreamsDidChangeNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:StreamRemovalRequestNotfication object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UINib *nib = [UINib nibWithNibName:@"PlaybackCollectionViewCell" bundle:nil];
    [self.collectionView registerNib:nib forCellWithReuseIdentifier:@"PlaybackCollectionViewCell"];
}

- (void)traitCollectionDidChange:(UITraitCollection *)previousTraitCollection {
    [self.collectionView.collectionViewLayout invalidateLayout];
}

#pragma mark - Segues

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"AddressesSegue"]) {
        UINavigationController *navController = (UINavigationController *)segue.destinationViewController;
        AddressesTableViewController *addressesController = navController.viewControllers.firstObject;
        if (addressesController && [addressesController isKindOfClass:[AddressesTableViewController class]]) {
            addressesController.playbackManager = self.playbackManager;
        }
    }
}

- (IBAction)unwind:(UIStoryboardSegue *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - UICollectionViewDataSource

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.playbackManager.streams.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    PlaybackCollectionViewCell *cell = (PlaybackCollectionViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@"PlaybackCollectionViewCell"
                                                                                                               forIndexPath:indexPath];
    cell.stream = self.playbackManager.streams[indexPath.item];
    cell.index = indexPath.item;
    return cell;
}

#pragma mark - UICollectionViewFlowLayoutDelegate

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlowLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    if (self.playbackManager.streams.count == 2) {
        return CGSizeMake(CGRectGetWidth(collectionView.bounds) - collectionViewLayout.minimumInteritemSpacing,
                          CGRectGetHeight(collectionView.bounds) * 0.5f - collectionViewLayout.minimumLineSpacing);
    }
    
    NSUInteger maxCols = 2;
    CGFloat colWidth = roundf(CGRectGetWidth(collectionView.bounds) / maxCols) - collectionViewLayout.minimumInteritemSpacing;
    
    NSUInteger totalRows = ceil((self.playbackManager.streams.count * 1.0) / (maxCols * 1.0));
    CGFloat rowHeight = roundf(CGRectGetHeight(collectionView.bounds) / totalRows) - collectionViewLayout.minimumLineSpacing;
    
    BOOL uneven = self.playbackManager.streams.count % maxCols != 0;
    BOOL lastRow = self.playbackManager.streams.count - indexPath.item == 1;
    return CGSizeMake(lastRow && uneven ? CGRectGetWidth(collectionView.bounds) : colWidth, rowHeight);
}

#pragma mark - Actions

- (void)removeRtspStreamFromNotification:(NSNotification *)notification {
    NSNumber *streamIndex = notification.object;
    if (![streamIndex isKindOfClass:[NSNumber class]]) {
        return;
    }
    [self.playbackManager removeStreamAtIndex:streamIndex.unsignedIntegerValue];
}

#pragma mark - Interface

- (void)reloadStreams {
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
    [self updateInterface];
}

- (void)updateInterface {
    UIColor *bgColor = self.playbackManager.streams.count ? [UIColor blackColor] : [UIColor whiteColor];
    [UIView animateWithDuration:0.35f animations:^{
        self.view.backgroundColor = bgColor;
    }];
}


@end
