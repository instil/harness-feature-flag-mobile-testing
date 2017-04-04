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
    if (response == nullptr) {
        return NULL;
    }

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

    jboolean isNative = (jboolean)sessionDescription.IsNative();
    jint framerate = sessionDescription.GetFramerate();
    jint fpsFraction = sessionDescription.GetFpsFraction();
    jint width = sessionDescription.GetWidth();
    jint height = sessionDescription.GetHeight();
    jint bitrate = sessionDescription.GetBitrate();

    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID constructor = env->GetMethodID(sessionDescriptionClass, "<init>", "(Lco/instil/surge/client/SessionType;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZIIIII)V");
    return env->NewObject(sessionDescriptionClass, constructor, type, controlUrl, formatParameters, rtpMap, isNative, framerate, fpsFraction, width, height, bitrate);
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
    if (response == nullptr) {
        return NULL;
    }

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


jclass jSessionDescription(JNIEnv *env) {
    return env->FindClass("co/instil/surge/client/SessionDescription");
}

std::string callAndConvertStringGetterOn(jclass javaClass, JNIEnv *env, const char * methodName, jobject jObjectToCall) {
    jmethodID method = env->GetMethodID(javaClass, methodName, "()Ljava/lang/String;");
    return SurgeJni::JavaTypeConverters::convertString(env, (jstring)env->CallObjectMethod(jObjectToCall, method));
}

bool callAndConvertBooleanGetterOn(jclass javaClass, JNIEnv *env, const char * methodName, jobject jObjectToCall) {
    jmethodID method = env->GetMethodID(javaClass, methodName, "()Z");
    return (bool)env->CallBooleanMethod(jObjectToCall, method);
}

int callAndConvertIntGetterOn(jclass javaClass, JNIEnv *env, const char * methodName, jobject jObjectToCall) {
    jmethodID method = env->GetMethodID(javaClass, methodName, "()I");
    return (int)env->CallIntMethod(jObjectToCall, method);
}



Surge::SessionDescription SurgeJni::JavaTypeConverters::convertSessionDescription(JNIEnv *env, jobject javaSessionDescription) {
    Surge::RtspSessionType type = convertSessionType(env, javaSessionDescription);

    std::string controlUrl = callAndConvertStringGetterOn(jSessionDescription(env), env, "getControlUrl", javaSessionDescription);
    std::string rtpMap = callAndConvertStringGetterOn(jSessionDescription(env), env, "getRtpMap", javaSessionDescription);
    std::string formatParameters = callAndConvertStringGetterOn(jSessionDescription(env), env, "getFormatParameters", javaSessionDescription);

    bool isNative = callAndConvertBooleanGetterOn(jSessionDescription(env), env, "isNative", javaSessionDescription);
    int framerate = callAndConvertIntGetterOn(jSessionDescription(env), env, "getFramerate", javaSessionDescription);
    int fpsFraction = callAndConvertIntGetterOn(jSessionDescription(env), env, "getFpsFraction", javaSessionDescription);
    int width = callAndConvertIntGetterOn(jSessionDescription(env), env, "getWidth", javaSessionDescription);
    int height = callAndConvertIntGetterOn(jSessionDescription(env), env, "getHeight", javaSessionDescription);
    int bitrate = callAndConvertIntGetterOn(jSessionDescription(env), env, "getBitrate", javaSessionDescription);

    Surge::SessionDescription sessionDescription(type, controlUrl, rtpMap, formatParameters, isNative, framerate, fpsFraction, width, height, bitrate);
    return sessionDescription;
}

SurgeUtil::DateTime SurgeJni::JavaTypeConverters::convertDate(JNIEnv *env, jobject jDate) {
    jclass dateClass = env->FindClass("java/util/Date");
    jmethodID getTimeMethod = env->GetMethodID(dateClass, "getTime", "()J");
    jlong jTimestamp = env->CallLongMethod(jDate, getTimeMethod);

    time_t timestamp = (time_t)(jTimestamp / 1000);
    struct tm *timeInfo = std::gmtime(&timestamp);

    SurgeUtil::DateTime surgeDateTime = SurgeUtil::DateTime();
    surgeDateTime.Year = timeInfo->tm_year + 1900;  // tm_year == years since 1900
    surgeDateTime.Month = timeInfo->tm_mon + 1;     // tm_mon starts at 0, Java and Surge start at 1
    surgeDateTime.Day = timeInfo->tm_mday;
    surgeDateTime.Hour = timeInfo->tm_hour;
    surgeDateTime.Minute = timeInfo->tm_min;
    surgeDateTime.Second = timeInfo->tm_sec;
    surgeDateTime.Nanosecond = 0;

    return surgeDateTime;
}