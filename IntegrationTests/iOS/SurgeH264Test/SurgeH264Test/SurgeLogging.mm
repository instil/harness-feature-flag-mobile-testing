//
//  SurgeLogging.mm
//  SurgeH264Test
//
//  Created by Philip Herron on 07/03/2016.
//  Copyright © 2016 Philip Herron. All rights reserved.
//

#import "SurgeLogging.h"

#include "Logging.h"
#include "LoggingDelegate.h"

class RtspLoggingDelegate: public SurgeUtil::LoggingDelegate {
public:
    
    void info(const char *message) const override {
        NSLog(@"INFO: %s", message);
    }
    
    void error(const char *message) const override {
        NSLog(@"ERROR: %s", message);
    }
    
    void fatal(const char *message) const override {
        NSLog(@"FATAL: %s", message);
    }
    
    void warn(const char *message) const override {
        NSLog(@"WARN: %s", message);
    }
    
};

@implementation SurgeLogging

+ (void)ConfigureLogging {
    static RtspLoggingDelegate *delegate = new RtspLoggingDelegate();
    
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingDelegate(delegate);
}

@end
