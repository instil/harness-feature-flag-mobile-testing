//
//  AddressesTableViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "AddressesTableViewController.h"
#import "PlaybackViewController.h"
#import "NSArray+RtspAddressStorage.h"
#import "Constants.h"

@interface AddressesTableViewController () <UISearchBarDelegate>
@property (nonatomic, weak) IBOutlet UISearchBar *searchBar;
@property (nonatomic, weak) IBOutlet UIBarButtonItem *addButton;
@property (nonatomic, copy) NSArray <NSString *> *allStoredAddresses;
@property (nonatomic, copy) NSArray <NSString *> *storedAddressSearchResults;
@end

@implementation AddressesTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.allStoredAddresses = [NSArray<NSString *> storedRtspAddresses];
    self.storedAddressSearchResults = self.allStoredAddresses;
}

#pragma mark - UISearchBarDelegate

- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText {
    self.addButton.enabled = searchText.length;
    if (searchText.length) {
        NSPredicate *predicate = [NSPredicate predicateWithFormat:@"self CONTAINS[cd] %@", searchBar.text];
        self.storedAddressSearchResults = [self.allStoredAddresses filteredArrayUsingPredicate:predicate];
    }
    else {
        self.storedAddressSearchResults = self.allStoredAddresses;
    }
    [self.tableView reloadData];
}

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [searchBar resignFirstResponder];
}

#pragma mark - Actions

- (IBAction)addButtonAction:(id)sender {
    self.allStoredAddresses = [self.allStoredAddresses arrayByAddingObject:self.searchBar.text];
    [self.allStoredAddresses saveAsStoredRtspAddresses];
    self.searchBar.text = @"";
    self.storedAddressSearchResults = self.allStoredAddresses;
    [self.searchBar resignFirstResponder];
    [self.tableView reloadData];
}

#pragma mark - Segues

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"AddressSelectionSegue"]) {
        NSString *selection = self.storedAddressSearchResults[self.tableView.indexPathForSelectedRow.row];
        PlaybackViewController *playbackViewController = (PlaybackViewController *)segue.destinationViewController;
        playbackViewController.playbackUrlString = selection;
    }
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.storedAddressSearchResults.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"AddressTableViewCell"];
    cell.textLabel.text = self.storedAddressSearchResults[indexPath.row];
    return cell;
}

#pragma mark - UITableViewDelegate

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
    NSString *address = self.storedAddressSearchResults[indexPath.row];
    NSMutableArray *m_storedAddressSearchResults = self.storedAddressSearchResults.mutableCopy;
    [m_storedAddressSearchResults removeObject:address];
    self.storedAddressSearchResults = m_storedAddressSearchResults;
    
    NSMutableArray *m_allStoredAddresses = self.allStoredAddresses.mutableCopy;
    [m_allStoredAddresses removeObject:address];
    self.allStoredAddresses = m_allStoredAddresses;
    [self.allStoredAddresses saveAsStoredRtspAddresses];
    
    [self.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationLeft];
}

@end
