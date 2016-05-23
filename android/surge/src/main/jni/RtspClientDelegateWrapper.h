#ifndef ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
#define ANDROID_RTSPCLIENTDELEGATEWRAPPER_H

#include <jni.h>
#include <Surge.h>

namespace SurgeJni {

    class RtspClientDelegateWrapper : public Surge::IRtspClientDelegate {

    public:
        RtspClientDelegateWrapper(JavaVM *jvm, jobject jDelegate) : jvm(jvm), jDelegate(jDelegate) {}

        void ClientDidTimeout();

        void StreamConfigChanged(bool wasRedirect);

        void Payload(const char* buffer, size_t length);

        jobject GetJavaDelegate() {
            return jDelegate;
        }

    private:
        JavaVM *jvm;
        jobject jDelegate;
    };

}

#endif //ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
