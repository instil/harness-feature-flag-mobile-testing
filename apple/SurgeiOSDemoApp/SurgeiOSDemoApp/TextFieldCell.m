//
//  TextFieldCell.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "TextFieldCell.h"

@interface TextFieldCell()
@property (nonatomic, strong) UITextField *textField;
@end

@implementation TextFieldCell

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super initWithCoder:coder];
  if (self) {
    _textField = [[UITextField alloc] initWithFrame:CGRectZero];
    _textField.textAlignment = NSTextAlignmentRight;
    _textField.tintColor = [UIColor blackColor];
    [self.contentView addSubview:_textField];
    
    self.detailTextLabel.hidden = YES;
  }
  return self;
}

- (void)layoutSubviews {
  [super layoutSubviews];
  CGFloat width = CGRectGetWidth(self.contentView.bounds) - CGRectGetMaxX(self.textLabel.frame) - 30;
  self.textField.frame = CGRectMake(CGRectGetMaxX(self.textLabel.frame) + 10, 0, width, 44);
}

@end
