//
//  NSArray+RtspAddressStorage.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 28/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RtspAddress.h"

@interface NSArray (RtspAddressStorage)
+ (NSArray<RtspAddress *> *)storedRtspAddresses;
- (BOOL)saveAsStoredRtspAddresses;
@end
