// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "TypeConverters.h"


jstring SurgeJni::NativeTypeConverters::convertString(JNIEnv *env, std::string string) {
    return env->NewStringUTF(string.c_str());
}

jobject SurgeJni::NativeTypeConverters::convertMap(SurgeJni::ClassLoader *classLoader, std::map<std::string, std::string> map) {
    JNIEnv *env = classLoader->AttachToJvm();

    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID constructor = env->GetMethodID(mapClass, "<init>", "()V");
    jobject jMap = env->NewObject(mapClass, constructor);
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    for (auto iterator = map.begin(); iterator != map.end(); iterator++) {
        jstring key = convertString(env, iterator->first);
        jstring value = convertString(env, iterator->second);
        env->CallObjectMethod(jMap, putMethod, key, value);
    }

    classLoader->DetachFromJvm();

    return jMap;
}

jobject SurgeJni::NativeTypeConverters::convertResponse(SurgeJni::ClassLoader *classLoader, Surge::RtspResponse *response) {
    if (response == nullptr) {
        return NULL;
    }

    JNIEnv *env = classLoader->AttachToJvm();

    jclass responseClass = classLoader->findClass("co/instil/surge/client/Response");
    jmethodID constructor = env->GetMethodID(responseClass, "<init>", "(ILjava/util/Map;Ljava/lang/String;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(classLoader, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());

    jobject result = env->NewObject(responseClass, constructor, statusCode, headers, body);
    classLoader->DetachFromJvm();

    return result;
}

jobject SurgeJni::NativeTypeConverters::convertSessionType(SurgeJni::ClassLoader *classLoader, Surge::RtspSessionType type) {
    JNIEnv *env = classLoader->AttachToJvm();

    const char *typeName;
    switch (type) {
        case Surge::RtspSessionType::H264: typeName = "H264"; break;
        case Surge::RtspSessionType::H265: typeName = "H265"; break;
        case Surge::RtspSessionType::MP4V: typeName = "MP4V"; break;
        case Surge::RtspSessionType::MJPEG: typeName = "MJPEG"; break;
        default: typeName = "UNKNOWN"; break;
    }
    jclass sessionTypeClass = classLoader->findClass("co/instil/surge/client/SessionType");
    jfieldID sessionType = env->GetStaticFieldID(sessionTypeClass , typeName, "Lco/instil/surge/client/SessionType;");

    jobject result = env->GetStaticObjectField(sessionTypeClass, sessionType);
    classLoader->DetachFromJvm();

    return result;
}

jobject SurgeJni::NativeTypeConverters::convertSessionDescription(SurgeJni::ClassLoader *classLoader, Surge::SessionDescription sessionDescription) {
    JNIEnv *env = classLoader->AttachToJvm();

    jobject type = convertSessionType(classLoader, sessionDescription.GetType());
    jstring controlUrl = env->NewStringUTF(sessionDescription.GetControl().c_str());
    jstring formatParameters = env->NewStringUTF(sessionDescription.GetFmtp().c_str());
    jstring rtpMap = env->NewStringUTF(sessionDescription.GetRtpMap().c_str());

    jboolean isNative = (jboolean)sessionDescription.IsNative();
    jint framerate = sessionDescription.GetFramerate();
    jint fpsFraction = sessionDescription.GetFpsFraction();
    jint width = sessionDescription.GetWidth();
    jint height = sessionDescription.GetHeight();
    jint bitrate = sessionDescription.GetBitrate();
    jint payloadType = sessionDescription.GetPayloadType();

    jclass sessionDescriptionClass = classLoader->findClass("co/instil/surge/client/SessionDescription");
    jmethodID constructor = env->GetMethodID(sessionDescriptionClass, "<init>", "(Lco/instil/surge/client/SessionType;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IZIIIII)V");

    jobject result = env->NewObject(sessionDescriptionClass, constructor, type, controlUrl, formatParameters, rtpMap, payloadType, isNative, framerate, fpsFraction, width, height, bitrate);
    classLoader->DetachFromJvm();

    return result;
}

jobject SurgeJni::NativeTypeConverters::convertSessionDescriptions(SurgeJni::ClassLoader *classLoader, std::vector<Surge::SessionDescription> sessionDescriptions) {
    JNIEnv *env = classLoader->AttachToJvm();

    jclass sessionDescriptionClass = classLoader->findClass("co/instil/surge/client/SessionDescription");
    jobjectArray jSessionDescriptions = env->NewObjectArray(sessionDescriptions.size(), sessionDescriptionClass, nullptr);
    for (int i = 0; i < sessionDescriptions.size(); i++) {
        env->SetObjectArrayElement(jSessionDescriptions, i, convertSessionDescription(classLoader, sessionDescriptions[i]));
    }

    classLoader->DetachFromJvm();

    return jSessionDescriptions;
}

jobject SurgeJni::NativeTypeConverters::convertDescribeResponse(SurgeJni::ClassLoader *classLoader, Surge::DescribeResponse *response) {
    if (response == nullptr) {
        return NULL;
    }

    JNIEnv *env = classLoader->AttachToJvm();

    jclass cls = classLoader->findClass("co/instil/surge/client/DescribeResponse");
    jmethodID  constructor = env->GetMethodID(cls, "<init>", "(ILjava/util/Map;Ljava/lang/String;[Lco/instil/surge/client/SessionDescription;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(classLoader, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());

    jobject result = env->NewObject(cls, constructor, statusCode, headers, body, convertSessionDescriptions(classLoader, response->GetSessionDescriptions()));
    classLoader->DetachFromJvm();

    return result;
}

jobject SurgeJni::NativeTypeConverters::convertRtpPacketBufferInfo(SurgeJni::ClassLoader *classLoader, Surge::RtpPacketBufferInfo &info) {
    JNIEnv *env = classLoader->AttachToJvm();

    jclass cls = classLoader->findClass("co/instil/surge/diagnostics/RtpPacketBufferInfo");
    jmethodID  constructor = env->GetMethodID(cls, "<init>", "(JJJ)V");
    jlong successfulPacketsCount = info.successfulPacketsCount;
    jlong missedPacketsCount = info.missedPacketsCount;
    jlong oooPacketsCount = info.oooPacketsCount;

    jobject result = env->NewObject(cls, constructor, successfulPacketsCount, missedPacketsCount, oooPacketsCount);
    classLoader->DetachFromJvm();

    return result;
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
    } else if (strcmp(typeString, "H265") == 0) {
        type = Surge::RtspSessionType::H265;
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
    int payloadType = callAndConvertIntGetterOn(jSessionDescription(env), env, "getPayloadType", javaSessionDescription);

    Surge::SessionDescription sessionDescription(type, controlUrl, rtpMap, formatParameters, isNative, framerate, fpsFraction, width, height, bitrate, payloadType);
    return sessionDescription;
}

SurgeUtil::DateTime* SurgeJni::JavaTypeConverters::convertDate(JNIEnv *env, jobject jDate) {
    if (jDate == NULL) {
        return nullptr;
    }

    jclass dateClass = env->FindClass("java/util/Date");
    jmethodID getTimeMethod = env->GetMethodID(dateClass, "getTime", "()J");
    jlong jTimestamp = env->CallLongMethod(jDate, getTimeMethod);

    time_t timestamp = (time_t)(jTimestamp / 1000);
    struct tm *timeInfo = std::gmtime(&timestamp);

    return new SurgeUtil::DateTime(
            timeInfo->tm_year + 1900,  // tm_year == years since 1900
            timeInfo->tm_mon + 1,     // tm_mon starts at 0, Java and Surge start at 1
            timeInfo->tm_mday,
            timeInfo->tm_hour,
            timeInfo->tm_min,
            timeInfo->tm_sec,
            0);
}

std::vector<char> SurgeJni::JavaTypeConverters::convertByteArrayToVector(ClassLoader *classLoader, jbyteArray jByteArray) {
    jboolean isCopy;
    JNIEnv *env = classLoader->AttachToJvm();

    int length = env->GetArrayLength(jByteArray);
    std::vector<char> result(length);
    env->GetByteArrayRegion(jByteArray, 0, length, (jbyte*)result.data());

    classLoader->DetachFromJvm();

    return result;
}

void SurgeJni::JavaTypeConverters::callResponseCallback(SurgeJni::ClassLoader *classLoader, jobject jResponseCallback, jobject jResponse) {
    JNIEnv *env = classLoader->AttachToJvm();

    jclass cls = classLoader->findClass("co/instil/surge/callbacks/ResponseCallback");
    jmethodID method = env->GetMethodID(cls, "response", "(Lco/instil/surge/client/Response;)V");
    env->CallVoidMethod(jResponseCallback, method, jResponse);

    classLoader->DetachFromJvm();
}

void SurgeJni::JavaTypeConverters::callBooleanCallback(SurgeJni::ClassLoader *classLoader, jobject jResponseCallback, jboolean jResponse) {
    JNIEnv *env = classLoader->AttachToJvm();

    jclass cls = classLoader->findClass("co/instil/surge/callbacks/BooleanCallback");
    jmethodID method = env->GetMethodID(cls, "response", "(Z)V");
    env->CallVoidMethod(jResponseCallback, method, jResponse);

    classLoader->DetachFromJvm();
}
