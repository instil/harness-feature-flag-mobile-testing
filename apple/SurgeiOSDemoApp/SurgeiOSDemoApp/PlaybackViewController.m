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

@interface PlaybackViewController () <UICollectionViewDataSource, UICollectionViewDelegate>
@property (nonatomic, weak) IBOutlet UICollectionView *collectionView;
@property (nonatomic, strong) NSMutableArray<NSString *> *urls;
@end

@implementation PlaybackViewController

- (instancetype)initWithCoder:(NSCoder *)coder {
    self = [super initWithCoder:coder];
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(loadRtspStreamFromNotification:)
                                                     name:RtspAddressSelectionNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(removeRtspStreamFromNotification:)
                                                     name:StreamRemovalRequestNotfication
                                                   object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:RtspAddressSelectionNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:StreamRemovalRequestNotfication object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.urls = [NSMutableArray<NSString *> new];
    
    UINib *nib = [UINib nibWithNibName:@"PlaybackCollectionViewCell" bundle:nil];
    [self.collectionView registerNib:nib forCellWithReuseIdentifier:@"PlaybackCollectionViewCell"];
}

- (void)traitCollectionDidChange:(UITraitCollection *)previousTraitCollection {
    [self.collectionView.collectionViewLayout invalidateLayout];
}

#pragma mark - Segues

- (IBAction)unwind:(UIStoryboardSegue *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - UICollectionViewDataSource

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.urls.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    PlaybackCollectionViewCell *cell = (PlaybackCollectionViewCell *)[collectionView dequeueReusableCellWithReuseIdentifier:@"PlaybackCollectionViewCell"
                                                                                                               forIndexPath:indexPath];
    cell.playbackUrlString = self.urls[indexPath.item];
    cell.index = indexPath.item;
    return cell;
}

#pragma mark - UICollectionViewFlowLayoutDelegate

- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewFlowLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    if (self.urls.count == 2) {
        return CGSizeMake(CGRectGetWidth(collectionView.bounds) - collectionViewLayout.minimumInteritemSpacing,
                          CGRectGetHeight(collectionView.bounds) * 0.5f - collectionViewLayout.minimumLineSpacing);
    }
    
    NSUInteger maxCols = 2;
    CGFloat colWidth = roundf(CGRectGetWidth(collectionView.bounds) / maxCols) - collectionViewLayout.minimumInteritemSpacing;
    
    NSUInteger totalRows = ceil((self.urls.count * 1.0) / (maxCols * 1.0));
    CGFloat rowHeight = roundf(CGRectGetHeight(collectionView.bounds) / totalRows) - collectionViewLayout.minimumLineSpacing;
    
    BOOL uneven = self.urls.count % maxCols != 0;
    BOOL lastRow = self.urls.count - indexPath.item == 1;
    return CGSizeMake(lastRow && uneven ? CGRectGetWidth(collectionView.bounds) : colWidth, rowHeight);
}

#pragma mark - Actions

- (void)loadRtspStreamFromNotification:(NSNotification *)notification {
    NSString *rtspAddress = notification.object;
    if (![rtspAddress isKindOfClass:[NSString class]]) {
        return;
    }
    [self.urls addObject:rtspAddress];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
    [self updateInterface];
}

- (void)removeRtspStreamFromNotification:(NSNotification *)notification {
    NSNumber *rtspAddressIndex = notification.object;
    if (![rtspAddressIndex isKindOfClass:[NSNumber class]]) {
        return;
    }
    [self.urls removeObjectAtIndex:rtspAddressIndex.unsignedIntegerValue];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
    [self updateInterface];
}

#pragma mark - Interface

- (void)updateInterface {
    UIColor *bgColor = self.urls.count ? [UIColor blackColor] : [UIColor whiteColor];
    [UIView animateWithDuration:0.35f animations:^{
        self.view.backgroundColor = bgColor;
    }];
}


@end
