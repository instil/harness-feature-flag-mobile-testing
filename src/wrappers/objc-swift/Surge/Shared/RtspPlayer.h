//
//  RtspPlayer.h
//  Surge
//
//  Created by Chris van Es on 29/03/2016.
//
//

#import <Foundation/Foundation.h>

@protocol RtspPlayerDelegate <NSObject>

@optional

- (void)rtpsPlayerDidBeginBuffering;

- (void)rtpsPlayerDidEndBuffering;

@end

@interface RtspPlayer : NSObject

@property (nonatomic, weak) id<RtspPlayerDelegate> delegate;

- (void)initiatePlayback:(NSURL*)url;

- (void)play;

- (void)pause;

- (void)stop;

@end
