//
//  AddressesViewController.h
//  SurgeMacOSDemoApp
//
//  Created by Niall Kelly on 29/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AddressesViewController : NSViewController <NSTableViewDelegate, NSTableViewDataSource>
@property (weak) IBOutlet NSTableView *tableView;
@end
