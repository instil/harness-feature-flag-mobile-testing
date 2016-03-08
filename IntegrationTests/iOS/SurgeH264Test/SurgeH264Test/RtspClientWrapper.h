//
//  RtspClientWrapper.h
//  SurgeH264Test
//
//  Created by Philip Herron on 07/03/2016.
//  Copyright © 2016 Philip Herron. All rights reserved.
//

#ifndef __RTSP_CLIENT_WRAPPER_H__
#define __RTSP_CLIENT_WRAPPER_H__

#import <Foundation/Foundation.h>

@protocol RtspClientDelegate <NSObject>

- (void)Timeout;

- (void)Payload:(const unsigned char*)payload withSize:(size_t)length;

@end

@interface RtspClientWrapper : NSObject

- (id)initWithUrl:(NSString*)url andDelegate:(id<RtspClientDelegate>)delegate;

- (void)playStream;

@end

#endif //__RTSP_CLIENT_WRAPPER_H__
