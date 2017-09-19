//
//  PlaybackCollectionViewCell.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 19/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <SurgeiOS/SurgeiOS.h>

@interface PlaybackCollectionViewCell : UICollectionViewCell
@property (readonly, nonatomic, strong) SurgeRtspPlayer *rtspPlayer;
@property (copy, nonatomic) NSString *playbackUrlString;
@end
