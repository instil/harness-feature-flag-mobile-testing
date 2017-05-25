// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController <NSSearchFieldDelegate>

@property (weak) IBOutlet NSImageView *playbackView;
@property (weak) IBOutlet NSSearchField *searchField;

@end

