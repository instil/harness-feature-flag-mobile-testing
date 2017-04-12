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

#ifndef ANDROID_TYPECONVERTERS_H
#define ANDROID_TYPECONVERTERS_H

#include <jni.h>
#include <Surge.h>
#include "ClassLoader.h"

namespace SurgeJni {

    namespace NativeTypeConverters {

        jstring convertString(JNIEnv *env, std::string string);

        jobject convertMap(SurgeJni::ClassLoader *classLoader, std::map<std::string, std::string> map);

        jobject convertResponse(SurgeJni::ClassLoader *classLoader, Surge::RtspResponse *response);

        jobject convertSessionType(SurgeJni::ClassLoader *classLoader, Surge::RtspSessionType type);

        jobject convertSessionDescription(SurgeJni::ClassLoader *classLoader, Surge::SessionDescription sessionDescription);

        jobject convertSessionDescriptions(SurgeJni::ClassLoader *classLoader, std::vector<Surge::SessionDescription> sessionDescriptions);

        jobject convertDescribeResponse(SurgeJni::ClassLoader *classLoader, Surge::DescribeResponse *response);

    }

    namespace JavaTypeConverters {

        std::string convertString(JNIEnv *env, jstring jString);

        Surge::RtspSessionType convertSessionType(JNIEnv *env, jobject jSessionDescription);

        Surge::SessionDescription convertSessionDescription(JNIEnv *env, jobject jSessionDescription);

        SurgeUtil::DateTime convertDate(JNIEnv *env, jobject jDate);

        void callResponseCallback(SurgeJni::ClassLoader *classLoader, jobject jResponseCallback, jobject jResponse);
    }
}


#endif //ANDROID_TYPECONVERTERS_H
