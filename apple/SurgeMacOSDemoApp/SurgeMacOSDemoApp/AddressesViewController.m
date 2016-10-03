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
@property (weak) IBOutlet NSButton *addButton;
@property (weak) IBOutlet NSSearchField *searchField;
@end

@implementation AddressesViewController

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:NSControlTextDidChangeNotification object:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.allStoredAddresses = [NSArray storedRtspAddresses];
    [self.arrayController addObjects:self.allStoredAddresses];
    self.addButton.enabled = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(controlTextDidChangeNotification:)
                                                 name:NSControlTextDidChangeNotification
                                               object:nil];
}

- (void)viewWillAppear {
    [super viewWillAppear];
    [self.tableView deselectAll:nil];
}

#pragma mark - Table

- (IBAction)rowSelectionChanged:(id)sender {
    NSInteger row = self.tableView.selectedRow;
    if (row >= 0) {
        [[NSNotificationCenter defaultCenter] postNotificationName:RtspAddressSelectionNotification object:self.arrayController.arrangedObjects[row]];
        [self.tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
    }
}

#pragma mark - Search

- (void)controlTextDidChangeNotification:(NSNotification *)notification {
    self.addButton.enabled = self.searchField.stringValue.length;
}

- (IBAction)addButtonAction:(id)sender {
    [self.arrayController addObject:self.searchField.stringValue];
    [self.arrayController.arrangedObjects saveAsStoredRtspAddresses];
}

#pragma mark - Menu

- (IBAction)menuDeleteItemAction:(id)sender {
    NSInteger clickedRow = self.tableView.clickedRow;
    
    NSString *address = self.arrayController.arrangedObjects[clickedRow];
    [self.arrayController removeObject:address];
    
    NSMutableArray *m_allStoredAddresses = self.allStoredAddresses.mutableCopy;
    [m_allStoredAddresses removeObject:address];
    self.allStoredAddresses = m_allStoredAddresses;
    [self.allStoredAddresses saveAsStoredRtspAddresses];
}

@end
