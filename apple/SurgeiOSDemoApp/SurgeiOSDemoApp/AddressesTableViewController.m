//
//  AddressesTableViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 30/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "AddressesTableViewController.h"
#import "AddAddressViewController.h"
#import "AddressSearchResultsTableViewController.h"
#import "NSArray+RtspAddressStorage.h"

NSString *const RtspAddressSelectionNotification = @"RtspAddressSelectionNotification";

@interface AddressesTableViewController () <UISearchBarDelegate>
@property (nonatomic, weak) IBOutlet UIBarButtonItem *addButton;
@property (strong, nonatomic) UISearchController *searchController;
@property (strong, nonatomic) AddressSearchResultsTableViewController *searchResultsController;
@end

@implementation AddressesTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UIStoryboard *sb = [UIStoryboard storyboardWithName:@"Main-iPhone" bundle:nil];
    self.searchResultsController = [sb instantiateViewControllerWithIdentifier:@"AddressSearchResultsTableViewController"];
    
    self.searchController = [[UISearchController alloc] initWithSearchResultsController:self.searchResultsController];
    self.searchController.searchBar.delegate = self;
    self.searchController.searchResultsUpdater = self.searchResultsController;
    self.searchController.searchBar.textContentType = UITextContentTypeURL;
    self.searchController.searchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
    self.searchController.searchBar.returnKeyType = UIReturnKeyDone;
    [self.searchController.searchBar sizeToFit];
    self.searchController.searchBar.tintColor = [UIColor whiteColor];
    self.searchController.searchBar.translucent = NO;
    self.definesPresentationContext = YES;
    
    if (@available(iOS 11.0, *)) {
        self.navigationItem.searchController = self.searchController;
        self.navigationItem.hidesSearchBarWhenScrolling = NO;
    }
    else {
        self.searchController.searchBar.barTintColor = self.navigationController.navigationBar.barTintColor;
        self.tableView.tableHeaderView = self.searchController.searchBar;
    }
}

#pragma mark - UISearchBarDelegate

- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
    [searchBar resignFirstResponder];
}

#pragma mark - Actions

- (IBAction)addButtonAction:(UIBarButtonItem *)sender {
    AddAddressViewController *addController = [[AddAddressViewController alloc] init];
    
    __weak typeof(self) weakSelf = self;
    [addController setCompletionHandler:^(RtspAddress *address) {
        [weakSelf addAddress:address];
        [self.tableView insertRowsAtIndexPaths:@[[NSIndexPath indexPathForRow:[self.tableView numberOfRowsInSection:0] inSection:0]]
                              withRowAnimation:UITableViewRowAnimationAutomatic];
    }];
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:addController];
    navController.navigationBar.barTintColor = self.navigationController.navigationBar.barTintColor;
    navController.navigationBar.tintColor = self.navigationController.navigationBar.tintColor;
    navController.modalPresentationStyle = UIModalPresentationPopover;
    navController.popoverPresentationController.barButtonItem = sender;
    [self presentViewController:navController animated:YES completion:nil];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.searchResultsController.allStoredAddresses.count;
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
                                                        object:[self addressAtIndexPath:indexPath].address];
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
    RtspAddress *address = self.searchResultsController.allStoredAddresses[indexPath.row];
    [self removeAddress:address];
    [self.tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationLeft];
}

#pragma mark - Helpers

- (RtspAddress *)addressAtIndexPath:(NSIndexPath *)indexPath {
    return self.searchResultsController.allStoredAddresses[indexPath.row];
}

- (void)addAddress:(RtspAddress *)address {
    NSMutableArray *m_allStoredAddresses = self.searchResultsController.allStoredAddresses.mutableCopy;
    [m_allStoredAddresses addObject:address];
    self.searchResultsController.allStoredAddresses = m_allStoredAddresses;
    [self.searchResultsController.allStoredAddresses saveAsStoredRtspAddresses];
    [self.searchResultsController.tableView reloadData];
}

- (void)removeAddress:(RtspAddress *)address {
    NSMutableArray *m_allStoredAddresses = self.searchResultsController.allStoredAddresses.mutableCopy;
    [m_allStoredAddresses removeObject:address];
    self.searchResultsController.allStoredAddresses = m_allStoredAddresses;
    [self.searchResultsController.allStoredAddresses saveAsStoredRtspAddresses];
    [self.searchResultsController.tableView reloadData];
}

@end
