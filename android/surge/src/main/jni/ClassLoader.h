// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef ANDROID_CLASSLOADER_H
#define ANDROID_CLASSLOADER_H

#include <jni.h>
#include <cstdlib>


namespace SurgeJni {

    /*
     * Allows Surge to load classes without the use of JNIEnv by directly calling the Java ClassLoader.
     *
     * Required for asynchronous JNI applications as the JNIEnv only has access to local app Java classes
     * in the main thread. This ClassLoader method works on all threads.
     */
    class ClassLoader {

    public:
        static ClassLoader *Instance() {
            return classLoaderSingleton;
        }

        static void AttachToJvm(JNIEnv *jniEnv) {
            if (classLoaderSingleton != nullptr) {
                delete classLoaderSingleton;
            }

            classLoaderSingleton = new ClassLoader(jniEnv);
        }

    private:
        static ClassLoader *classLoaderSingleton;

    public:
        jclass findClass(const char* name);
        JNIEnv* getEnv();

    private:
        ClassLoader(JNIEnv *env);
        ~ClassLoader();

        void GenerateDetachKeyForJvm() {
            pthread_key_create(&detachKey, DetatchJvmThread);
        }

        void DetachCurrentThread() {
            jvm->DetachCurrentThread();
        }

        static void DetatchJvmThread(void *rawEnv) {
            DEBUG("Detaching JVM Thread");
            classLoaderSingleton->DetachCurrentThread();
        }

    private:
        JavaVM *jvm;
        jobject classLoader;
        jmethodID findClassMethod;
        pthread_key_t detachKey;
    };
}

#endif //ANDROID_CLASSLOADER_H
