//
//  AddressSearchResultsTableViewController.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RtspAddress;

@interface AddressSearchResultsTableViewController : UITableViewController <UISearchResultsUpdating>
@property (nonatomic, copy) NSArray <RtspAddress *> *allStoredAddresses;
@property (nonatomic, copy) NSArray <RtspAddress *> *searchResults;
@end
