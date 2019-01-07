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

        SurgeUtil::DateTime* convertDate(JNIEnv *env, jobject jDate);

        std::vector<char> convertByteArrayToVector(ClassLoader *classLoader, jbyteArray jByteArray);

        void callResponseCallback(SurgeJni::ClassLoader *classLoader, jobject jResponseCallback, jobject jResponse);

        void callBooleanCallback(SurgeJni::ClassLoader *classLoader, jobject jResponseCallback, jboolean jResponse);
    }

    namespace JavaTypeConverters {

        template <class T>
        std::vector<T> convertList(SurgeJni::ClassLoader *classLoader, jobject jList, std::function<T(jobject)> innerConverter) {
            JNIEnv *env = classLoader->AttachToJvm();

            jclass listClass = env->FindClass("java/util/List");
            jmethodID listSizeMethod = env->GetMethodID(listClass, "size", "()I");
            jmethodID listGetMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

            jint listSize = env->CallIntMethod(jList, listSizeMethod);

            std::vector<T> result;
            result.reserve((unsigned long)listSize);

            for (jint i = 0; i < listSize; i++) {
                jobject jItem = env->CallObjectMethod(jList, listGetMethod, i);
                T item = innerConverter(jItem);
                result.emplace_back(item);
            }

            classLoader->DetachFromJvm();

            return result;
        }

        template <class T>
        std::vector<T> convertList(SurgeJni::ClassLoader *classLoader, jobject jList) {
            return convertList<T>(classLoader, jList, [](jobject jItem) {
                return (T)jItem;
            });

        }
    }
}


#endif //ANDROID_TYPECONVERTERS_H
