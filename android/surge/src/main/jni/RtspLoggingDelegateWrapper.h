#ifndef ANDROID_RTSPLOGGINGDELEGATEWRAPPER_H
#define ANDROID_RTSPLOGGINGDELEGATEWRAPPER_H

#include "Logging.h"

namespace SurgeJni {

    class RtspLoggingDelegateWrapper : public SurgeUtil::ILoggingDelegate {

    public:
        void trace(const char *message) const override;

        void debug(const char *message) const override;

        void info(const char *message) const override;

        void warn(const char *message) const override;

        void error(const char *message) const override;

        void fatal(const char *message) const override;

    };

}
#endif //ANDROID_RTSPLOGGINGDELEGATEWRAPPER_H
