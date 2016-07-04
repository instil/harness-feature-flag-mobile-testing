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

#include "TypeConverters.h"


jstring SurgeJni::NativeTypeConverters::convertString(JNIEnv *env, std::string string) {
    return env->NewStringUTF(string.c_str());
}

jobject SurgeJni::NativeTypeConverters::convertMap(JNIEnv *env, std::map<std::string, std::string> map) {
    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID constructor = env->GetMethodID(mapClass, "<init>", "()V");
    jobject jMap = env->NewObject(mapClass, constructor);
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    for (auto iterator = map.begin(); iterator != map.end(); iterator++) {
        jstring key = convertString(env, iterator->first);
        jstring value = convertString(env, iterator->second);
        env->CallObjectMethod(jMap, putMethod, key, value);
    }
    return jMap;
}

jobject SurgeJni::NativeTypeConverters::convertResponse(JNIEnv *env, Surge::RtspResponse *response) {
    jclass responseClass = env->FindClass("co/instil/surge/client/Response");
    jmethodID constructor = env->GetMethodID(responseClass, "<init>", "(ILjava/util/Map;Ljava/lang/String;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(env, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());
    return env->NewObject(responseClass, constructor, statusCode, headers, body);
}

jobject SurgeJni::NativeTypeConverters::convertSessionType(JNIEnv *env, Surge::RtspSessionType type) {
    const char *typeName;
    switch (type) {
        case Surge::RtspSessionType::H264: typeName = "H264"; break;
        case Surge::RtspSessionType::MP4V: typeName = "MP4V"; break;
        case Surge::RtspSessionType::MJPEG: typeName = "MJPEG"; break;
        default: typeName = "UNKNOWN"; break;
    }
    jclass sessionTypeClass = env->FindClass("co/instil/surge/client/SessionType");
    jfieldID sessionType = env->GetStaticFieldID(sessionTypeClass , typeName, "Lco/instil/surge/client/SessionType;");
    return env->GetStaticObjectField(sessionTypeClass, sessionType);
}

jobject SurgeJni::NativeTypeConverters::convertSessionDescription(JNIEnv *env, Surge::SessionDescription sessionDescription) {
    jobject type = convertSessionType(env, sessionDescription.GetType());
    jstring controlUrl = env->NewStringUTF(sessionDescription.GetControl().c_str());
    jstring formatParameters = env->NewStringUTF(sessionDescription.GetFmtp().c_str());
    jstring rtpMap = env->NewStringUTF(sessionDescription.GetRtpMap().c_str());
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID constructor = env->GetMethodID(sessionDescriptionClass, "<init>", "(Lco/instil/surge/client/SessionType;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    return env->NewObject(sessionDescriptionClass, constructor, type, controlUrl, formatParameters, rtpMap);
}

jobject SurgeJni::NativeTypeConverters::convertSessionDescriptions(JNIEnv *env, std::vector<Surge::SessionDescription> sessionDescriptions) {
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jobjectArray jSessionDescriptions = env->NewObjectArray(sessionDescriptions.size(), sessionDescriptionClass, nullptr);
    for (int i = 0; i < sessionDescriptions.size(); i++) {
        env->SetObjectArrayElement(jSessionDescriptions, i, convertSessionDescription(env, sessionDescriptions[i]));
    }
    return jSessionDescriptions;
}

jobject SurgeJni::NativeTypeConverters::convertDescribeResponse(JNIEnv *env, Surge::DescribeResponse *response) {
    jclass cls = env->FindClass("co/instil/surge/client/DescribeResponse");
    jmethodID  constructor = env->GetMethodID(cls, "<init>", "(ILjava/util/Map;Ljava/lang/String;[Lco/instil/surge/client/SessionDescription;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(env, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());
    return env->NewObject(cls, constructor, statusCode, headers, body, convertSessionDescriptions(env, response->GetSessionDescriptions()));
}

std::string SurgeJni::JavaTypeConverters::convertString(JNIEnv *env, jstring jString) {
    const char * chars = env->GetStringUTFChars(jString, JNI_FALSE);
    std::string string(chars);
    env->ReleaseStringUTFChars(jString, chars);
    return string;
}

Surge::RtspSessionType SurgeJni::JavaTypeConverters::convertSessionType(JNIEnv *env, jobject jSessionDescription) {
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID getTypeMethod = env->GetMethodID(sessionDescriptionClass, "getType", "()Lco/instil/surge/client/SessionType;");
    jobject typeEnumValue = env->CallObjectMethod(jSessionDescription, getTypeMethod);
    jclass sessionTypeClass = env->FindClass("co/instil/surge/client/SessionType");
    jmethodID nameMethod = env->GetMethodID(sessionTypeClass, "name", "()Ljava/lang/String;");
    jstring typeJString = (jstring)env->CallObjectMethod(typeEnumValue, nameMethod);

    Surge::RtspSessionType type = Surge::RtspSessionType::UNKNOWN;
    const char * typeString = env->GetStringUTFChars(typeJString, JNI_FALSE);
    if (strcmp(typeString, "H264") == 0) {
        type = Surge::RtspSessionType::H264;
    } else if (strcmp(typeString, "MP4V") == 0) {
        type = Surge::RtspSessionType::MP4V;
    } else if (strcmp(typeString, "MJPEG") == 0) {
        type = Surge::RtspSessionType::MJPEG;
    }
    env->ReleaseStringUTFChars(typeJString, typeString);
    return type;
}

jstring callJSessionDescriptionGetter(JNIEnv *env, jobject jSessionDescription, const char * methodName) {
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID method = env->GetMethodID(sessionDescriptionClass, methodName, "()Ljava/lang/String;");
    return (jstring)env->CallObjectMethod(jSessionDescription, method);
}

Surge::SessionDescription SurgeJni::JavaTypeConverters::convertSessionDescription(JNIEnv *env, jobject jSessionDescription) {
    Surge::RtspSessionType type = convertSessionType(env, jSessionDescription);
    std::string controlUrl = convertString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getControlUrl"));
    std::string rtpMap = convertString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getRtpMap"));
    std::string formatParameters = convertString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getFormatParameters"));
    Surge::SessionDescription sessionDescription(type, controlUrl, rtpMap, formatParameters, -1, -1, -1);
    return sessionDescription;
}