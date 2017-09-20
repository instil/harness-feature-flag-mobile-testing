//
//  PlaybackCollectionViewCell.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <SurgeiOS/SurgeiOS.h>

extern NSString *const StreamRemovalRequestNotfication;

@interface PlaybackCollectionViewCell : UICollectionViewCell
@property (copy, nonatomic) NSString *playbackUrlString;
@property (nonatomic) NSUInteger index;
@end
