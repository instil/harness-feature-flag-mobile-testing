// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
