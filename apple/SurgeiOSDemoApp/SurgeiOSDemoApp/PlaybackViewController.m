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
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:RtspAddressSelectionNotification object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.urls = [NSMutableArray<NSString *> new];
    
    UINib *nib = [UINib nibWithNibName:@"PlaybackCollectionViewCell" bundle:nil];
    [self.collectionView registerNib:nib forCellWithReuseIdentifier:@"PlaybackCollectionViewCell"];
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
    [self playUrl:rtspAddress];
}

- (void)playUrl:(NSString *)urlString {
    [self.urls addObject:urlString];
    [self.collectionView reloadSections:[NSIndexSet indexSetWithIndex:0]];
//    [self.rtspPlayer initiatePlaybackOf:[NSURL URLWithString:urlString] withUsername:@"admin" andPassword:@"admin"];
}
//
//- (void)rtspPlayerFailedToInitiatePlayback:(nonnull SurgeRtspPlayer *)player {
//    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Oops"
//                                                                   message:@"Failed to start stream"
//                                                            preferredStyle:UIAlertControllerStyleAlert];
//    [alert addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
//        [self.navigationController popToRootViewControllerAnimated:YES];
//    }]];
//    [self presentViewController:alert animated:YES completion:nil];
//}



@end
