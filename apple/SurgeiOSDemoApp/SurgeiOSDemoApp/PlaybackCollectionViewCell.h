//
//  PlaybackCollectionViewCell.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>

extern NSString *const StreamRemovalRequestNotfication;

@class PlaybackStream;

@interface PlaybackCollectionViewCell : UICollectionViewCell
@property (copy, nonatomic) PlaybackStream *stream;
@property (nonatomic) NSUInteger index;
@end
