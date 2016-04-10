// -*-c++-*-
#ifndef __LOGGING_DELEGATE_H__
#define __LOGGING_DELEGATE_H__

namespace SurgeUtil {

    class LoggingDelegate {
    public:

        virtual ~LoggingDelegate() { }

        virtual void info(const char *message) const { }

        virtual void error(const char *message) const { }

        virtual void debug(const char *message) const { }

        virtual void fatal(const char *message) const { }

        virtual void warn(const char *message) const { }

        virtual void trace(const char *message) const { }
        
    };
    
};

#endif //__LOGGING_DELEGATE_H__
