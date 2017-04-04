// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "RtspClientDelegateWrapper.h"


void SurgeJni::RtspClientDelegateWrapper::ClientDidTimeout() {
    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientDidTimeout", "()V");
    env->CallVoidMethod(jDelegate, method);
    jvm->DetachCurrentThread();
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
    jvm->AttachCurrentThread(&env, NULL);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedFrame", "(Ljava/nio/ByteBuffer;IIII)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, length);
    env->CallVoidMethod(jDelegate, method, framebuffer, width, height, presentationTime, duration);
    env->DeleteLocalRef(framebuffer);
    env->DeleteLocalRef(cls);
    jvm->DetachCurrentThread();
}
void SurgeJni::RtspClientDelegateWrapper::ClientReceivedExtendedHeader(const unsigned char *buffer, size_t size) {

    JNIEnv *env;
    jvm->AttachCurrentThread(&env, NULL);
    jclass cls = env->GetObjectClass(jDelegate);
    jmethodID method = env->GetMethodID(cls, "clientReceivedExtendedHeader", "(Ljava/nio/ByteBuffer;I)V");
    jobject framebuffer = env->NewDirectByteBuffer((void*)buffer, size);
    env->CallVoidMethod(jDelegate, method, framebuffer, size);
    env->DeleteLocalRef(framebuffer);
    env->DeleteLocalRef(cls);
    jvm->DetachCurrentThread();
}

