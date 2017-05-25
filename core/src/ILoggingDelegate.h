// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __ILOGGING_DELEGATE_H__
#define __ILOGGING_DELEGATE_H__

namespace SurgeUtil {

    class ILoggingDelegate {
    public:

        virtual ~ILoggingDelegate() { }

        virtual void info(const char *message) const { }

        virtual void error(const char *message) const { }

        virtual void debug(const char *message) const { }

        virtual void fatal(const char *message) const { }

        virtual void warn(const char *message) const { }

        virtual void trace(const char *message) const { }
        
    };
    
};

#endif //__LOGGING_DELEGATE_H__
