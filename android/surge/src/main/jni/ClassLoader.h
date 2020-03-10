// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef ANDROID_CLASSLOADER_H
#define ANDROID_CLASSLOADER_H

#include <jni.h>
#include <cstdlib>

#include <android/log.h>
#include <string>


namespace SurgeJni {

    /*
     * Allows Surge to load classes without the use of JNIEnv by directly calling the Java ClassLoader.
     *
     * Required for asynchronous JNI applications as the JNIEnv only has access to local app Java classes
     * in the main thread. This ClassLoader method works on all threads.
     */
    class ClassLoader {

    public:
        ClassLoader(JNIEnv *env);
        ~ClassLoader();

    public:
        jclass findClass(const char* name);
        JNIEnv* AttachToJvm();
        void DetachFromJvm();

    private:
        void TrackNewThreadAttachedToJvm() {
            ++jniEnvCount;
        }

        void TrackExistingJniEnvAcquired() {
            if (jniEnvCount == 0) {
                jniEnvCount = -1;
            }
            ++jniEnvCount;
        }

        bool ShouldDetachJniEnv() {
            return jniEnvCount == -1 ? false : --jniEnvCount == 0;
        }


    private:
        JavaVM *jvm;
        thread_local static int jniEnvCount;

        jobject classLoader;
        jmethodID findClassMethod;
    };
}

#endif //ANDROID_CLASSLOADER_H
