//
//  RtspAddress.h
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RtspAddress : NSObject <NSCoding>
+ (nullable instancetype)withAddress:(nonnull NSString *)address;
+ (nullable instancetype)withAddress:(nonnull NSString *)address
                                name:(nullable NSString *)name;
+ (nullable instancetype)withAddress:(nonnull NSString *)address
                                name:(nullable NSString *)name
                            username:(nullable NSString *)username
                            password:(nullable NSString *)password;
@property (readonly, copy, nonatomic, nonnull) NSString *address;
@property (readonly, copy, nonatomic, nullable) NSString *name;
@property (readonly, copy, nonatomic, nullable) NSString *username;
@property (readonly, copy, nonatomic, nullable) NSString *password;
@end
