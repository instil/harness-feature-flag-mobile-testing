#include "RtspClientDelegateWrapper.h"

void SurgeJni::RtspClientDelegateWrapper::ClientDidTimeout() {
    jclass cls = env->GetObjectClass(delegate);
    jmethodID method = env->GetMethodID(cls, "clientDidTimeout", "()V");
    env->CallVoidMethod(delegate, method);
}

void SurgeJni::RtspClientDelegateWrapper::StreamConfigChanged(bool wasRedirect) {

}

void SurgeJni::RtspClientDelegateWrapper::Payload(const char* buffer, size_t length) {
    jclass cls = env->GetObjectClass(delegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedFrame", "(Ljava/nio/ByteBuffer;)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, length);
    env->CallVoidMethod(delegate, method, framebuffer);
}
