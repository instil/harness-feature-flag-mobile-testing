// Copyright (c) 2018 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#import "SurgeDiagnosticsDelegate.h"
#import "SurgeRtspPlayerDelegate.h"
#import "SurgeDiagnostics.h"

@interface DiagnosticsTracker : NSObject<SurgeDiagnostics>

- (id)initWithPlayer:(nonnull SurgeRtspPlayer *)player;

- (void)startTracking;
- (void)stopTracking;

- (void)trackNewFrameOfSize:(size_t)bitCountInFrame;
- (void)trackNewFrameDimensionsWithWidth:(size_t)width andHeight:(size_t)height;
- (void)trackNewMediaFormat:(int)mediaFormat;

- (void)updateDelegate;

@property (nonatomic, weak, nullable) id<SurgeDiagnosticsDelegate> diagnosticsDelegate;
@property (nonatomic, strong, nullable) id<SurgeRtspPlayerDelegate> depreciatedDiagnosticsDelegate;

@end
