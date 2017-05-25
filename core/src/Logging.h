// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#include "ILoggingDelegate.h"

namespace SurgeUtil {

    enum class LogLevel { Trace = 0, Debug, Info, Warning, Error, Fatal };

    class Logger {
    public:
        
        static Logger& GetInstance() {
            static Logger instance;
            return instance;
        }

        void SetLevel(LogLevel level) { m_level = level; }

        void SetLoggingDelegate(const ILoggingDelegate *delegate) { m_delegate = delegate;  }
        
        void Log(const LogLevel a_level,
                 const std::ostringstream& a_message,
                 const char* a_file,
                 const char* a_functon,
                 const int a_line) {
            
            if (!IsLogLevelEnabled(a_level)) {
                return;
            }

            if (m_delegate == nullptr) {
                return;
            }

            std::ostringstream stream;
            stream << a_file << ":" << a_line << " -> " << a_message.str();
            std::string cppMessage = stream.str();
            const char * message = cppMessage.c_str();

            switch (a_level)
            {
            case LogLevel::Trace:
                m_delegate->trace(message);
                break;

            case LogLevel::Debug:
                m_delegate->debug(message);
                break;

            case LogLevel::Info:
                m_delegate->info(message);
                break;

            case LogLevel::Warning:
                m_delegate->warn(message);
                break;

            case LogLevel::Error:
                m_delegate->error(message);
                break;

            case LogLevel::Fatal:
                m_delegate->fatal(message);
                break;
            }
        }

    private:
        Logger(): m_level(LogLevel::Debug), m_delegate(nullptr) { }

        bool IsLogLevelEnabled(LogLevel level) const {
            return level >= m_level;
        }

        LogLevel m_level;
        const ILoggingDelegate *m_delegate;
    };

}

// iOS defines DEBUG...
#undef DEBUG

// This will remove the very long FILEPATH to the __FILE__ making logging too big
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG(level, message)                                             \
    do {                                                                \
        {                                                               \
            std::ostringstream stream;                                  \
            stream << message;                                          \
            SurgeUtil::Logger logger = SurgeUtil::Logger::GetInstance(); \
            logger.Log(level, stream, __FILENAME__, __FUNCTION__, __LINE__); \
        }} while (false)

#define TRACE(message)   LOG(SurgeUtil::LogLevel::Trace,   message)
#define DEBUG(message)   LOG(SurgeUtil::LogLevel::Debug,   message)
#define INFO(message)    LOG(SurgeUtil::LogLevel::Info,    message)
#define WARNING(message) LOG(SurgeUtil::LogLevel::Warning, message)
#define ERROR(message)   LOG(SurgeUtil::LogLevel::Error,   message)
#define FATAL(message)   LOG(SurgeUtil::LogLevel::Fatal,   message)

#endif //__LOGGING_H__
