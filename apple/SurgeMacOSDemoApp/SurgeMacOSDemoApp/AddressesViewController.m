//
//  AddressesViewController.m
//  SurgeMacOSDemoApp
//
//  Created by Niall Kelly on 29/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "AddressesViewController.h"
#import "NSArray+RtspAddressStorage.h"
#import "Constants.h"

@interface AddressesViewController ()
@property (copy) NSArray <NSString *> *allStoredAddresses;
@property (strong) IBOutlet NSArrayController *arrayController;
@end

@implementation AddressesViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.allStoredAddresses = [NSArray storedRtspAddresses];
    [self.arrayController addObjects:self.allStoredAddresses];
}

- (void)viewWillAppear {
    [super viewWillAppear];
    [self.tableView deselectAll:nil];
}

- (IBAction)rowSelectionChanged:(id)sender {
    NSInteger row = self.tableView.selectedRow;
    if (row >= 0) {
        [[NSNotificationCenter defaultCenter] postNotificationName:RtspAddressSelectionNotification object:self.arrayController.arrangedObjects[row]];
        [self.tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
    }
}

@end
