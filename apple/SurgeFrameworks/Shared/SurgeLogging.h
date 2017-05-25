// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#import <Foundation/Foundation.h>

#ifdef DEBUG
#define SurgeLogDebug(...) NSLog(@"DEBUG: " __VA_ARGS__)
#else
#define SurgeLogDebug(...)
#endif

#define SurgeLogInfo(...) NSLog(@"INFO: " __VA_ARGS__)
#define SurgeLogWarn(...) NSLog(@"WARN: " __VA_ARGS__)
#define SurgeLogError(...) NSLog(@"ERROR: " __VA_ARGS__)
