//
//  AddAddressViewController.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "AddAddressViewController.h"
#import "TextFieldCell.h"
#import "RtspAddress.h"

@interface AddAddressViewController () <UITextFieldDelegate>
@property (copy, nonatomic) NSArray *titles;
@end

@implementation AddAddressViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  self.title = @"Add Address";
  self.titles = @[@"Address", @"Name", @"Username", @"Password"];
  self.preferredContentSize = CGSizeMake(320, 320);
  self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
  self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel
                                                                                        target:self
                                                                                        action:@selector(cancelButtonAction:)];
  self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone
                                                                                         target:self
                                                                                         action:@selector(doneButtonAction:)];
  [self checkCompletionWithText:@""];
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
  if (_completionHandler) {
    UITextField *addressField = [self textFieldAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
    UITextField *nameField = [self textFieldAtIndexPath:[NSIndexPath indexPathForRow:1 inSection:0]];
    UITextField *usernameField = [self textFieldAtIndexPath:[NSIndexPath indexPathForRow:2 inSection:0]];
    UITextField *passwordField = [self textFieldAtIndexPath:[NSIndexPath indexPathForRow:3 inSection:0]];
    RtspAddress *rtspAddress = [RtspAddress withAddress:addressField.text
                                                   name:nameField.text
                                               username:usernameField.text
                                               password:passwordField.text];
    _completionHandler(rtspAddress);
  }
  [self.presentingViewController dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.titles.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  TextFieldCell *cell = [tableView dequeueReusableCellWithIdentifier:@"TextFieldCell" forIndexPath:indexPath];
  if (indexPath.row == 0) {
    cell.textField.textContentType = UITextContentTypeURL;
    cell.textField.keyboardType = UIKeyboardTypeURL;
    cell.textField.autocorrectionType = UITextAutocorrectionTypeNo;
    cell.textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
  }
  cell.textField.delegate = indexPath.row == 0 ? self : nil;
  cell.textField.placeholder = indexPath.row == 0 ? nil : @"Optional";
  cell.textLabel.text = self.titles[indexPath.row];
  return cell;
}

#pragma mark - UITextFieldDelegate

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
  __weak typeof(self) weakSelf = self;
  dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
    [weakSelf checkCompletionWithText:textField.text];
  });
  return YES;
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

- (void)checkCompletionWithText:(NSString *)text {
  self.navigationItem.rightBarButtonItem.enabled = text && text.length;
}

@end
