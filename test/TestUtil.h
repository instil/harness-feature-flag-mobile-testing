// -*-c++-*-
#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include "Logging.h"
#include "LoggingDelegate.h"

namespace SurgeTestUtil {

    void SetupTestLogger(SurgeUtil::LogLevel level = SurgeUtil::LogLevel::Debug);

    class Logger: public SurgeUtil::LoggingDelegate {
    public:
        
        void info(const char *message) const override {
            printf("INFO: %s\n", message);
        }

        void error(const char *message) const override {
            printf("ERROR: %s\n", message);
        }

        void debug(const char *message) const override {
            printf("DEBUG: %s\n", message);
        }

        void fatal(const char *message) const override {
            printf("FATAL: %s\n", message);
        }

        void warn(const char *message) const override {
            printf("WARN: %s\n", message);
        }

        void trace(const char *message) const override {
            printf("TRACE: %s\n", message);
        }

        // singleton
        static Logger& GetInstance() {
            static Logger logger;
            return logger;
        }
    };
    
}

#endif //__TEST_UTIL_H__
