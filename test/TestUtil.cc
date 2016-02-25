#include "TestUtil.h"

#include <string>

static void TEST_LOG_HANDLER(SurgeUtil::LogLevel level, const char *message) {
    std::string full_log_message = "";

    switch (level) {
    default:
        full_log_message += "LOG: ";
        break;
    }

    full_log_message += std::string(message);
    printf("%s\n", full_log_message.c_str());
}

void SurgeTestUtil::SetupTestLogger(SurgeUtil::LogLevel level) {
    SurgeUtil::LoggingHooks hooks;
    memset(&hooks, 0, sizeof(hooks));

    hooks.trace = &TEST_LOG_HANDLER;
    hooks.debug = &TEST_LOG_HANDLER;
    hooks.info = &TEST_LOG_HANDLER;
    hooks.warn = &TEST_LOG_HANDLER;
    hooks.error = &TEST_LOG_HANDLER;
    hooks.fatal = &TEST_LOG_HANDLER;

    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLevel(level);
    logger.SetLoggingHooks(hooks);
}
