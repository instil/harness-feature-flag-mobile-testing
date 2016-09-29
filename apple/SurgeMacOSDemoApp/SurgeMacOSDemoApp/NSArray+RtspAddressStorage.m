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
    NSArray <NSString *> *stored = [NSKeyedUnarchiver unarchiveObjectWithFile:[self rtspAddressesStoragePath]];
    if (stored) {
        return [self arrayWithArray:stored];
    }
    stored = @[@"rtsp://192.168.1.54:8554/test", @"rtsp://127.0.0.1:8554/test"];
    [stored saveAsStoredRtspAddresses];
    return [self arrayWithArray:stored];
}

- (void)saveAsStoredRtspAddresses {
    [NSKeyedArchiver archiveRootObject:self toFile:[NSArray<NSString *> rtspAddressesStoragePath]];
}

+ (NSString *)rtspAddressesStoragePath {
    NSURL *docsBaseUrl = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
    return [docsBaseUrl URLByAppendingPathComponent:@"rtspAddresses.plist"].absoluteString;
}

@end
