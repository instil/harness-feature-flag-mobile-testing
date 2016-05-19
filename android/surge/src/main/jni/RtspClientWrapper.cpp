#include "RtspClientWrapper.h"
#include "RtspClientDelegateWrapper.h"
#include <RtspClient.h>

JNIEXPORT
jlong JNICALL Java_co_instil_surge_client_RtspClient_createNativeClientInstance(JNIEnv *env, jobject callingObject, jobject delegate) {
    return reinterpret_cast<jlong>(new Surge::RtspClient(new SurgeJni::RtspClientDelegateWrapper(env, delegate)));
}

Surge::RtspClient* getNativeClientPointer(JNIEnv *env, jobject callingObject) {
    jclass c = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(c, "nativeClient", "J");
    jlong handle = env->GetLongField(callingObject, field);
    return reinterpret_cast<Surge::RtspClient*>(handle);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_options__(JNIEnv *env, jobject callingObject) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_options__Ljava_lang_String_2(JNIEnv *, jobject, jstring) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring jurl) {
    Surge::RtspClient *client = getNativeClientPointer(env, callingObject);
    const char *u = env->GetStringUTFChars(jurl, JNI_FALSE);
    std::string url = std::string(u);
    Surge::DescribeResponse *response = client->Describe(url);
    env->ReleaseStringUTFChars(jurl, u);

    jclass cls = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID constructor = env->GetMethodID(cls, "<init>", "(Lco/instil/surge/client/SessionType;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    std::vector<Surge::SessionDescription> sessionDescriptions = response->GetSessionDescriptions();
    jobjectArray jSessionDescriptions;
    for (int i = 0; i < sessionDescriptions.size(); i++) {
        const char *typeName;
        switch (sessionDescriptions[i].GetType()) {
            case Surge::RtspSessionType::H264: typeName = "H264"; break;
            case Surge::RtspSessionType::MP4V: typeName = "MP4V"; break;
            case Surge::RtspSessionType::MJPEG: typeName = "MJPEG"; break;
            default: typeName = "UNKNOWN"; break;
        }

        jclass sessionType = env->FindClass("co/instil/surge/client/SessionType");
        jfieldID typeEnumValue = env->GetStaticFieldID(sessionType , typeName, "Lco/instil/surge/client/SessionType;");
        jobject type = env->GetStaticObjectField(sessionType, typeEnumValue);
        jstring controlUrl = env->NewStringUTF(sessionDescriptions[i].GetControl().c_str());
        jstring formatParams = env->NewStringUTF(sessionDescriptions[i].GetFmtp().c_str());
        jstring rtpMap = env->NewStringUTF(sessionDescriptions[i].GetRtpMap().c_str());

        jobject jSessionDescription = env->NewObject(cls, constructor, type, controlUrl, formatParams, rtpMap);
        if (!jSessionDescriptions) {
            jSessionDescriptions = env->NewObjectArray(sessionDescriptions.size(), cls, jSessionDescription);
        } else {
            env->SetObjectArrayElement(jSessionDescriptions, i, jSessionDescription);
        }
    }

    cls = env->FindClass("co/instil/surge/client/DescribeResponse");
    constructor = env->GetMethodID(cls, "<init>", "([Lco/instil/surge/client/SessionDescription;)V");
    return env->NewObject(cls, constructor, jSessionDescriptions);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *, jobject, jstring, jstring, jstring) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *, jobject, jobject) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_play(JNIEnv *, jobject) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_pause(JNIEnv *, jobject) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_tearDown(JNIEnv *, jobject) {
    return nullptr;
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_keepAlive(JNIEnv *, jobject) {
    return nullptr;
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_close(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getNativeClientPointer(env, callingObject);
    delete client;
}
