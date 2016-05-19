#ifndef ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
#define ANDROID_RTSPCLIENTDELEGATEWRAPPER_H

#include <jni.h>
#include <Surge.h>

namespace SurgeJni {

class RtspClientDelegateWrapper : public Surge::IRtspClientDelegate {

public:
    RtspClientDelegateWrapper(JNIEnv *env, jobject delegate) : env(env), delegate(delegate) {}

    void ClientDidTimeout();

    void StreamConfigChanged(bool wasRedirect);

    void Payload(const char* buffer, size_t length);

private:
    JNIEnv *env;
    jobject delegate;
};

}

#endif //ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
