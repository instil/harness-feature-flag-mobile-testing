//
//  SurgeLogging.mm
//  SurgeH264Test
//
//  Created by Philip Herron on 07/03/2016.
//  Copyright © 2016 Philip Herron. All rights reserved.
//

#import "SurgeLogging.h"

#include "Logging.h"

void LogFunction(SurgeUtil::LogLevel level, const char *message) {
    NSLog(@"LOG: %s", message);
}

@implementation SurgeLogging

+ (void)ConfigureLogging {
    struct SurgeUtil::LoggingHooks hooks;
    
    hooks.info = &LogFunction;
    hooks.debug = &LogFunction;
    hooks.error = &LogFunction;
    hooks.fatal = &LogFunction;
    hooks.trace = &LogFunction;
    hooks.warn = &LogFunction;
    
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingHooks(hooks);
}

@end
