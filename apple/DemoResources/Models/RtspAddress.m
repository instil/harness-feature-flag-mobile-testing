//
//  RtspAddress.m
//  SurgeiOSDemoApp
//
//  Created by Niall Kelly on 21/09/2017.
//  Copyright © 2017 Instil. All rights reserved.
//

#import "RtspAddress.h"

@interface RtspAddress ()
@property (copy, nonatomic) NSString *address;
@property (copy, nonatomic) NSString *name;
@property (copy, nonatomic) NSString *username;
@property (copy, nonatomic) NSString *password;
@end

@implementation RtspAddress

+ (instancetype)withAddress:(nonnull NSString *)address {
  return [self withAddress:address name:nil];
}

+ (instancetype)withAddress:(nonnull NSString *)address name:(nullable NSString *)name {
  return [self withAddress:address name:name username:nil password:nil];
}

+ (nullable instancetype)withAddress:(nonnull NSString *)address name:(nullable NSString *)name username:(NSString *)username password:(NSString *)password {
  RtspAddress *obj = [[self alloc] init];
  obj.address = address;
  obj.name = name;
  obj.username = username;
  obj.password = password;
  return obj;
}

- (void)encodeWithCoder:(NSCoder *)aCoder {
  [aCoder encodeObject:self.address forKey:@"address"];
  if (self.name) {
    [aCoder encodeObject:self.name forKey:@"name"];
  }
  if (self.username) {
    [aCoder encodeObject:self.username forKey:@"username"];
  }
  if (self.password) {
    [aCoder encodeObject:self.password forKey:@"password"];
  }
}

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super init];
  if (self) {
    self.address = [coder decodeObjectForKey:@"address"];
    self.name = [coder decodeObjectForKey:@"name"];
    self.username = [coder decodeObjectForKey:@"username"];
    self.password = [coder decodeObjectForKey:@"password"];
  }
  return self;
}

@end
