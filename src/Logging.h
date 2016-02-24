// -*-c++-*-
#ifndef __LOGGING_H__
#define __LOGGING_H__

// Standard C++ Headers
#include <sstream>
#include <string>
#include <typeinfo>

namespace SurgeUtil {

    enum class LogLevel { Trace, Debug, Info, Warning, Error, Fatal };

    typedef void (*LOG_FUNCTION)(LogLevel level, const char *message);
    struct LoggingHooks {
        LOG_FUNCTION trace = 0;
        LOG_FUNCTION debug = 1;
        LOG_FUNCTION info = 2;
        LOG_FUNCTION warn = 3;
        LOG_FUNCTION error = 4;
        LOG_FUNCTION fatal = 5;
    };

    static struct LoggingHooks LoggingHooks;

    class Logger {
    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        void SetLevel(LogLevel level) { m_level = level; }
        
        void Log(const LogLevel a_level,
                 const std::ostringstream& a_message,
                 const char* a_file,
                 const char* a_functon,
                 const int a_line) {

            std::ostringstream stream;
            stream << a_file << ":" << a_line << " -> " << a_message.str();
            std::string cppMessage = stream.str();
            const char * message = cppMessage.c_str();

            switch (a_level)
            {
            case LogLevel::Trace:
                if (LoggingHooks.trace) {
                    LoggingHooks.trace(Trace, message);
                }
                break;

            case LogLevel::Debug:
                if (LoggingHooks.debug) {
                    LoggingHooks.debug(Debug, message);
                }
                break;

            case LogLevel::Info:
                if (LoggingHooks.info) {
                    LoggingHooks.info(Info, message);
                }
                break;

            case LogLevel::Warning:
                if (LoggingHooks.warn) {
                    LoggingHooks.warn(Warning, message);
                }
                break;

            case LogLevel::Error:
                if (LoggingHooks.error) {
                    LoggingHooks.error(Error, message);
                }
                break;

            case LogLevel::Fatal:
                if (LoggingHooks.fatal) {
                    LoggingHooks.fatal(Fatal, message);
                }
                break;
            }
        }

    private:
        Logger(): m_level(LogLevel.Debug) { }; // Empty CTOR

        bool IsLogLevelEnabled(LogLevel level) {
            return level >= m_level;
        }

        LogLevel m_level;
    };
        
}

// This will remove the very long FILEPATH to the __FILE__ making logging too big
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define _LOG(level, message)                                            \
    do {                                                                \
        {                                                               \
            std::ostringstream stream;                                  \
            stream << message;                                          \
            logger.Log(level, stream, __FILENAME__, __FUNCTION__, __LINE__); \
        }} while (false)

#define TRACE(message)   LOG(SurgeUtil::LogLevel::Trace,   message)
#define DEBUG(message)   LOG(SurgeUtil::LogLevel::Debug,   message)
#define INFO(message)    LOG(SurgeUtil::LogLevel::Info,    message)
#define WARNING(message) LOG(SurgeUtil::LogLevel::Warning, message)
#define ERROR(message)   LOG(SurgeUtil::LogLevel::Error,   message)
#define FATAL(message)   LOG(SurgeUtil::LogLevel::Fatal,   message)

#endif //__LOGGING_H__
