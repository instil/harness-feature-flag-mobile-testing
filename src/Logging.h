// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#include "LoggingDelegate.h"

namespace SurgeUtil {

    enum class LogLevel { Trace = 0, Debug, Info, Warning, Error, Fatal };

    class Logger {
    public:
        
        static Logger& GetInstance() {
            static Logger instance;
            return instance;
        }

        void SetLevel(LogLevel level) { m_level = level; }

        void SetLoggingDelegate(const LoggingDelegate *delegate) { m_delegate = delegate;  }
        
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

        bool IsLogLevelEnabled(LogLevel level) {
            return level >= m_level;
        }

        LogLevel m_level;
        const LoggingDelegate *m_delegate;
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
