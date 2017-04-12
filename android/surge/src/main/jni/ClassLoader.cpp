// Copyright (c) 2017 Instil Software.
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