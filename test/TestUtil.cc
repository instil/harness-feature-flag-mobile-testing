#include "TestUtil.h"

void SurgeTestUtil::SetupTestLogger(SurgeUtil::LogLevel level) {
    Logger& logger_handler = Logger::GetInstance();
    
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLevel(level);
    logger.SetLoggingDelegate(&logger_handler);
}
