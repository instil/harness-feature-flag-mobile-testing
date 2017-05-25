// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspClientDelegateWrapper.h"


JavaVM * SurgeJni::RtspClientDelegateWrapper::staticJvm;


void SurgeJni::RtspClientDelegateWrapper::ClientDidTimeout() {
    JNIEnv *env;
    getEnv(&env);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientDidTimeout", "()V");
    env->CallVoidMethod(jDelegate, method);
    env->DeleteLocalRef(cls);
}

void SurgeJni::RtspClientDelegateWrapper::StreamConfigChanged(bool wasRedirect) {

}

void SurgeJni::RtspClientDelegateWrapper::ClientReceivedFrame(const unsigned char * buffer,
                                                              size_t length,
                                                              int32_t width,
                                                              int32_t height,
                                                              int32_t presentationTime,
                                                              int32_t duration) {
    JNIEnv *env;
    getEnv(&env);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedFrame", "(Ljava/nio/ByteBuffer;IIII)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, length);
    env->CallVoidMethod(jDelegate, method, framebuffer, width, height, presentationTime, duration);
    env->DeleteLocalRef(framebuffer);
    env->DeleteLocalRef(cls);
}
void SurgeJni::RtspClientDelegateWrapper::ClientReceivedExtendedHeader(const unsigned char *buffer, size_t size) {

    JNIEnv *env;
    getEnv(&env);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedExtendedHeader", "(Ljava/nio/ByteBuffer;I)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, size);
    env->CallVoidMethod(jDelegate, method, framebuffer, size);
    env->DeleteLocalRef(framebuffer);
    env->DeleteLocalRef(cls);
}

