// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "ClassLoader.h"

SurgeJni::ClassLoader::ClassLoader(JNIEnv *jniEnv) {
    jniEnv->GetJavaVM(&this->jvm);

    jclass randomClass = jniEnv->FindClass("co/instil/surge/player/RtspPlayer");
    jclass classClass = jniEnv->GetObjectClass(randomClass);

    jclass classLoaderClass = jniEnv->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoaderMethod = jniEnv->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    this->classLoader = jniEnv->NewGlobalRef(jniEnv->CallObjectMethod(randomClass, getClassLoaderMethod));

    this->findClassMethod = jniEnv->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}

jclass SurgeJni::ClassLoader::findClass(const char* name) {
    JNIEnv *env = this->getEnv();
    jstring className = env->NewStringUTF(name);
    jclass result = static_cast<jclass>(env->CallObjectMethod(this->classLoader, this->findClassMethod, className));
    env->DeleteLocalRef(className);
    return result;
}

JNIEnv* SurgeJni::ClassLoader::getEnv() {
    JNIEnv *env;
    int status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    jvmToBeDetatched = false;

    if (status < 0) {
        status = jvm->AttachCurrentThread(&env, NULL);
        jvmToBeDetatched = true;

        if (status < 0) {
            return nullptr;
        }
    }

    return env;
}

void SurgeJni::ClassLoader::detatchJniEnv() {
    if (jvmToBeDetatched) {
        jvm->DetachCurrentThread();
    }
}