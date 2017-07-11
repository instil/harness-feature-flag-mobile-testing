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
        ClassLoader(JNIEnv *jniEnv);

        jclass findClass(const char* name);

        JNIEnv* getEnv();
        void detatchJniEnv();

    private:
        JavaVM *jvm;
        jobject classLoader;
        jmethodID findClassMethod;

        bool jvmToBeDetatched = false;
    };
}

#endif //ANDROID_CLASSLOADER_H
