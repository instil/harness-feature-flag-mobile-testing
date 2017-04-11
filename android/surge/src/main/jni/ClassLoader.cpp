//
// Created by Paul Shields on 10/04/2017.
//

#include "ClassLoader.h"

SurgeJni::ClassLoader::ClassLoader(JNIEnv *jniEnv) {
    jniEnv->GetJavaVM(&this->jvm);

    auto randomClass = jniEnv->FindClass("co/instil/surge/player/RtspPlayer");
    jclass classClass = jniEnv->GetObjectClass(randomClass);

    auto classLoaderClass = jniEnv->FindClass("java/lang/ClassLoader");
    auto getClassLoaderMethod = jniEnv->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    this->classLoader = jniEnv->NewGlobalRef(jniEnv->CallObjectMethod(randomClass, getClassLoaderMethod));

    this->findClassMethod = jniEnv->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}

jclass SurgeJni::ClassLoader::findClass(const char* name) {
    JNIEnv *env = this->getEnv();
    return static_cast<jclass>(env->CallObjectMethod(this->classLoader, this->findClassMethod, env->NewStringUTF(name)));
}

JNIEnv* SurgeJni::ClassLoader::getEnv() {
    JNIEnv *env;
    int status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status < 0) {
        status = jvm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            return nullptr;
        }
    }

    return env;
}