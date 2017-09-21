//
//  AddAddressViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "AddAddressViewController.h"
#import "TextFieldCell.h"

@interface AddAddressViewController ()

@end

@implementation AddAddressViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  self.title = @"Add Address";
  self.preferredContentSize = CGSizeMake(320, 320);
  self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
  self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel
                                                                                        target:self
                                                                                        action:@selector(cancelButtonAction:)];
  self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone
                                                                                         target:self
                                                                                         action:@selector(doneButtonAction:)];
  [self.tableView registerNib:[UINib nibWithNibName:@"TextFieldCell" bundle:nil] forCellReuseIdentifier:@"TextFieldCell"];
}

- (void)viewDidAppear:(BOOL)animated {
  [super viewDidAppear:animated];
  [[self textFieldAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]] becomeFirstResponder];
}

- (void)viewWillDisappear:(BOOL)animated {
  [super viewWillDisappear:animated];
  UITextField *currentFirstResponder = [self currentFirstResponder];
  if (currentFirstResponder) {
    [currentFirstResponder resignFirstResponder];
  }
}

#pragma mark - Actions

- (void)cancelButtonAction:(id)sender {
  [self.presentingViewController dismissViewControllerAnimated:YES completion:nil];
}

- (void)doneButtonAction:(id)sender {
  [self.presentingViewController dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  TextFieldCell *cell = [tableView dequeueReusableCellWithIdentifier:@"TextFieldCell" forIndexPath:indexPath];
  cell.textLabel.text = (indexPath.row == 1) ? @"Name" : @"Address";
  if (indexPath.row == 0) {
    cell.textField.textContentType = UITextContentTypeURL;
    cell.textField.keyboardType = UIKeyboardTypeURL;
    cell.textField.autocorrectionType = UITextAutocorrectionTypeNo;
    cell.textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
  }
  return cell;
}

#pragma mark - Helpers

- (UITextField *)textFieldAtIndexPath:(NSIndexPath *)indexPath {
  TextFieldCell *cell = (TextFieldCell *)[self.tableView cellForRowAtIndexPath:indexPath];
  return cell.textField;
}

- (UITextField *)currentFirstResponder {
  for (NSUInteger row=0; row<[self.tableView numberOfRowsInSection:0]; row++) {
    UITextField *textField = [self textFieldAtIndexPath:[NSIndexPath indexPathForRow:row inSection:0]];
    if (textField.isFirstResponder) {
      return textField;
    }
  }
  return nil;
}

@end
