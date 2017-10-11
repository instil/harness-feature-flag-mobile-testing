//
//  AddressesTableViewController.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PlaybackManager;

@interface AddressesTableViewController : UITableViewController
@property (strong, nonatomic) PlaybackManager *playbackManager;
@end
