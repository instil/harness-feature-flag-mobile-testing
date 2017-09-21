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
@end

@implementation RtspAddress

+ (instancetype)withAddress:(nonnull NSString *)address name:(nullable NSString *)name {
  RtspAddress *obj = [[self alloc] init];
  obj.address = address;
  obj.name = name;
  return obj;
}

- (void)encodeWithCoder:(NSCoder *)aCoder {
  [aCoder encodeObject:self.address forKey:@"address"];
  [aCoder encodeObject:self.name forKey:@"name"];
}

- (instancetype)initWithCoder:(NSCoder *)coder {
  self = [super init];
  if (self) {
    self.address = [coder decodeObjectForKey:@"address"];
    self.name = [coder decodeObjectForKey:@"name"];
  }
  return self;
}

@end
