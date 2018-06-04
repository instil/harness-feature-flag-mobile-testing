// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
#define ANDROID_RTSPCLIENTDELEGATEWRAPPER_H

#include <jni.h>
#include <Surge.h>

namespace SurgeJni {

    class RtspClientDelegateWrapper : public Surge::IRtspClientDelegate {

    public:
        pthread_key_t key;
        static JavaVM *staticJvm;

        static void detatchJvmThread(void *rawEnv) {
            DEBUG("Detaching JVM Thread");
            staticJvm->DetachCurrentThread();
        }

    public:
        RtspClientDelegateWrapper(JavaVM *jvm, jobject jDelegate) : jvm(jvm), jDelegate(jDelegate) {
            staticJvm = jvm;
            pthread_key_create(&key, detatchJvmThread);
        }

        void ClientDidTimeout();

        void StreamConfigChanged(bool wasRedirect);

        void ClientReceivedFrame(const unsigned char * buffer,
                                 size_t length,
                                 int32_t width,
                                 int32_t height,
                                 int32_t presentationTime,
                                 int32_t duration);

        void ClientReceivedExtendedHeader(const unsigned char *buffer,
                                          size_t size);

            jobject GetJavaDelegate() {
            return jDelegate;
        }

    private:
        JavaVM *jvm;
        jobject jDelegate;


        void getEnv(JNIEnv **env) {
            int status = jvm->GetEnv((void **) env, JNI_VERSION_1_6);
            if (status < 0) {
                INFO("Attaching thread to JVM");
                status = jvm->AttachCurrentThread(env, NULL);
                pthread_setspecific(key, &env);
            }
        }
    };

}

#endif //ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
