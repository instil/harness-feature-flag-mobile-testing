//
//  AddressSearchResultsTableViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "AddressSearchResultsTableViewController.h"
#import "NSArray+RtspAddressStorage.h"

@interface AddressSearchResultsTableViewController ()

@end

@implementation AddressSearchResultsTableViewController

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    _allStoredAddresses = [NSArray<RtspAddress *> storedRtspAddresses];
    _searchResults = @[];
  }
  return self;
}

#pragma mark - UISearchResultsUpdating

- (void)updateSearchResultsForSearchController:(UISearchController *)searchController {
  NSString *searchText = searchController.searchBar.text;
  if (searchText.length) {
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"self.name CONTAINS[cd] %@ || self.address CONTAINS[cd] %@", searchText, searchText];
    self.searchResults = [self.allStoredAddresses filteredArrayUsingPredicate:predicate];
  }
  else {
    self.searchResults = @[];
  }
  [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:0] withRowAnimation:UITableViewRowAnimationAutomatic];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.searchResults.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"AddressTableViewCell"];
  RtspAddress *address = [self addressAtIndexPath:indexPath];
  cell.textLabel.text = address.address;
  cell.detailTextLabel.text = address.name;
  return cell;
}

#pragma mark - Helpers

- (RtspAddress *)addressAtIndexPath:(NSIndexPath *)indexPath {
  return self.searchResults[indexPath.row];
}

@end
