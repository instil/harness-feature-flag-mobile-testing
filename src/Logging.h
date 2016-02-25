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
        LOG_FUNCTION trace;
        LOG_FUNCTION debug;
        LOG_FUNCTION info;
        LOG_FUNCTION warn;
        LOG_FUNCTION error;
        LOG_FUNCTION fatal;
    };

    class Logger {
    public:
        
        static Logger& GetInstance() {
            static Logger instance;
            return instance;
        }

        void SetLevel(LogLevel level) { m_level = level; }

        void SetLoggingHooks(LoggingHooks hooks) { m_loggingHooks = hooks; }
        
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
                if (m_loggingHooks.trace) {
                    m_loggingHooks.trace(LogLevel::Trace, message);
                }
                break;

            case LogLevel::Debug:
                if (m_loggingHooks.debug) {
                    m_loggingHooks.debug(LogLevel::Debug, message);
                }
                break;

            case LogLevel::Info:
                if (m_loggingHooks.info) {
                    m_loggingHooks.info(LogLevel::Info, message);
                }
                break;

            case LogLevel::Warning:
                if (m_loggingHooks.warn) {
                    m_loggingHooks.warn(LogLevel::Warning, message);
                }
                break;

            case LogLevel::Error:
                if (m_loggingHooks.error) {
                    m_loggingHooks.error(LogLevel::Error, message);
                }
                break;

            case LogLevel::Fatal:
                if (m_loggingHooks.fatal) {
                    m_loggingHooks.fatal(LogLevel::Fatal, message);
                }
                break;
            }
        }

    private:
        Logger(): m_level(LogLevel::Debug) { }; // Empty CTOR

        bool IsLogLevelEnabled(LogLevel level) {
            return level >= m_level;
        }

        LogLevel m_level;
        struct LoggingHooks m_loggingHooks;
    };
        
}

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
