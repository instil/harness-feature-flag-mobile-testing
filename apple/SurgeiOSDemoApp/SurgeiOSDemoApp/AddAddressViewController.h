//
//  AddAddressViewController.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RtspAddress;

typedef void(^AddAddressCompletionHandler)(RtspAddress *address);

@interface AddAddressViewController : UITableViewController
@property (nonatomic, copy) AddAddressCompletionHandler completionHandler;
@end
