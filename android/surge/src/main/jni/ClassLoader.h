//
// Created by Paul Shields on 10/04/2017.
//

#ifndef ANDROID_CLASSLOADER_H
#define ANDROID_CLASSLOADER_H

#include <jni.h>
#include <cstdlib>


namespace SurgeJni {

    class ClassLoader {

    public:
        ClassLoader(JNIEnv *jniEnv);

        jclass findClass(const char* name);

        JNIEnv* getEnv();

    private:
        JavaVM *jvm;
        jobject classLoader;
        jmethodID findClassMethod;

    };
}

#endif //ANDROID_CLASSLOADER_H
