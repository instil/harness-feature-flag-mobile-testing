//
//  AddressesTableViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "AddressesTableViewController.h"
#import "AddAddressViewController.h"
#import "NSArray+RtspAddressStorage.h"

NSString *const RtspAddressSelectionNotification = @"RtspAddressSelectionNotification";

@interface AddressesTableViewController () <UISearchBarDelegate, UISearchResultsUpdating>
@property (nonatomic, weak) IBOutlet UIBarButtonItem *addButton;
@property (strong, nonatomic) UISearchController *searchController;
@property (nonatomic, copy) NSArray <RtspAddress *> *allStoredAddresses;
@property (nonatomic, copy) NSArray <RtspAddress *> *storedAddressSearchResults;
@end

@implementation AddressesTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.allStoredAddresses = [NSArray<NSString *> storedRtspAddresses];
    self.storedAddressSearchResults = self.allStoredAddresses;
    
    self.searchController = [[UISearchController alloc] initWithSearchResultsController:nil];
    self.searchController.searchBar.delegate = self;
    self.searchController.searchResultsUpdater = self;
    self.searchController.searchBar.textContentType = UITextContentTypeURL;
    self.searchController.searchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
    self.searchController.searchBar.returnKeyType = UIReturnKeyDone;
    [self.searchController.searchBar sizeToFit];
    self.searchController.dimsBackgroundDuringPresentation = NO;
    self.searchController.searchBar.tintColor = [UIColor whiteColor];
    
    if (@available(iOS 11.0, *)) {
        self.navigationItem.searchController = self.searchController;
        self.navigationItem.hidesSearchBarWhenScrolling = NO;
    }
    else {
        self.searchController.searchBar.barTintColor = self.navigationController.navigationBar.barTintColor;
        self.tableView.tableHeaderView = self.searchController.searchBar;
        self.definesPresentationContext = YES;
    }
}

#pragma mark - UISearchResultsUpdating

- (void)updateSearchResultsForSearchController:(UISearchController *)searchController {
    NSString *searchText = searchController.searchBar.text;
    if (searchText.length) {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"self CONTAINS[cd] %@", searchText];
        self.storedAddressSearchResults = [self.allStoredAddresses filteredArrayUsingPredicate:predicate];
    }
    else {
        self.storedAddressSearchResults = self.allStoredAddresses;
    }
    [self.tableView reloadData];
}

#pragma mark - UISearchBarDelegate

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [searchBar resignFirstResponder];
}

#pragma mark - Actions

- (IBAction)addButtonAction:(UIBarButtonItem *)sender {
    AddAddressViewController *addController = [[AddAddressViewController alloc] init];
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:addController];
    navController.navigationBar.barTintColor = self.navigationController.navigationBar.barTintColor;
    navController.modalPresentationStyle = UIModalPresentationPopover;
    navController.popoverPresentationController.barButtonItem = sender;
    [self presentViewController:navController animated:YES completion:nil];
//    self.allStoredAddresses = [self.allStoredAddresses arrayByAddingObject:self.searchController.searchBar.text];
//    [self.allStoredAddresses saveAsStoredRtspAddresses];
//    self.storedAddressSearchResults = self.allStoredAddresses;
//    [self.searchController.searchBar resignFirstResponder];
//    [self.tableView reloadData];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.searchController.isActive ? self.storedAddressSearchResults.count : self.allStoredAddresses.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"AddressTableViewCell"];
    RtspAddress *address = [self addressAtIndexPath:indexPath];
    cell.textLabel.text = address.address;
    cell.detailTextLabel.text = address.name;
    return cell;
}

#pragma mark - UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [[NSNotificationCenter defaultCenter] postNotificationName:RtspAddressSelectionNotification
                                                        object:[self addressAtIndexPath:indexPath]];
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath {
    return UITableViewCellEditingStyleDelete;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle != UITableViewCellEditingStyleDelete) {
        return;
    }
    RtspAddress *address = self.storedAddressSearchResults[indexPath.row];
    NSMutableArray *m_storedAddressSearchResults = self.storedAddressSearchResults.mutableCopy;
    [m_storedAddressSearchResults removeObject:address];
    self.storedAddressSearchResults = m_storedAddressSearchResults;
    
    NSMutableArray *m_allStoredAddresses = self.allStoredAddresses.mutableCopy;
    [m_allStoredAddresses removeObject:address];
    self.allStoredAddresses = m_allStoredAddresses;
    [self.allStoredAddresses saveAsStoredRtspAddresses];
    
    [self.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationLeft];
}

#pragma mark - Helpers

- (RtspAddress *)addressAtIndexPath:(NSIndexPath *)indexPath {
    return self.allStoredAddresses[indexPath.row];
}

@end
