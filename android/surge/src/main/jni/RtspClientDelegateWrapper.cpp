#include "RtspClientDelegateWrapper.h"

void SurgeJni::RtspClientDelegateWrapper::ClientDidTimeout() {
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientDidTimeout", "()V");
    env->CallVoidMethod(jDelegate, method);
}

void SurgeJni::RtspClientDelegateWrapper::StreamConfigChanged(bool wasRedirect) {

}

void SurgeJni::RtspClientDelegateWrapper::Payload(const char* buffer, size_t length) {
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedFrame", "(Ljava/nio/ByteBuffer;)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, length);
    env->CallVoidMethod(jDelegate, method, framebuffer);
}
