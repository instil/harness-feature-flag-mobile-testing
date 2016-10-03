//
//  NSArray+RtspAddressStorage.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 28/09/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#import "NSArray+RtspAddressStorage.h"

@implementation NSArray (RtspAddressStorage)

+ (instancetype)storedRtspAddresses {
  NSURL *path = [self rtspAddressesStoragePath];
  NSData *data = [NSData dataWithContentsOfURL:path options:NSDataReadingUncached error:nil];
  if (data) {
    NSArray *stored = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    return [self arrayWithArray:stored];
  }
  NSArray *defaultValues = @[@"rtsp://192.168.1.54:8554/test", @"rtsp://127.0.0.1:8554/test"];
  [defaultValues saveAsStoredRtspAddresses];
  return [self arrayWithArray:defaultValues];
}

- (BOOL)saveAsStoredRtspAddresses {
  NSURL    *path = [NSArray rtspAddressesStoragePath];
  NSData *data = [NSKeyedArchiver archivedDataWithRootObject:self];
  return [data writeToURL:path atomically:YES];
}

+ (NSURL *)rtspAddressesStoragePath {
  NSURL *docsBaseUrl = [[[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] lastObject];
  return [docsBaseUrl URLByAppendingPathComponent:@"rtspAddresses.plist"];
}

@end
