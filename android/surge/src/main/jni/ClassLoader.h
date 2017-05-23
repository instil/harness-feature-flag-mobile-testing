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

    };
}

#endif //ANDROID_CLASSLOADER_H
