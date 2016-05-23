#include "RtspClientWrapper.h"
#include "RtspClientDelegateWrapper.h"
#include "RtspLoggingDelegateWrapper.h"


Surge::RtspClient* getClient(JNIEnv *env, jobject callingObject) {
    jclass callingObjectClass = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(callingObjectClass, "nativeClient", "J");
    jlong clientHandle = env->GetLongField(callingObject, field);
    return reinterpret_cast<Surge::RtspClient*>(clientHandle);
}

/*
 * Converting from C++ types to Java types.
 */

jstring convertString(JNIEnv *env, std::string string) {
    return env->NewStringUTF(string.c_str());
}

jobject convertMap(JNIEnv *env, std::map<std::string, std::string> map) {
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

jobject convertResponse(JNIEnv *env, Surge::RtspResponse *response) {
    jclass responseClass = env->FindClass("co/instil/surge/client/Response");
    jmethodID constructor = env->GetMethodID(responseClass, "<init>", "(ILjava/util/Map;Ljava/lang/String;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(env, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());
    return env->NewObject(responseClass, constructor, statusCode, headers, body);
}

jobject convertSessionType(JNIEnv *env, Surge::RtspSessionType type) {
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

jobject convertSessionDescription(JNIEnv *env, Surge::SessionDescription sessionDescription) {
    jobject type = convertSessionType(env, sessionDescription.GetType());
    jstring controlUrl = env->NewStringUTF(sessionDescription.GetControl().c_str());
    jstring formatParameters = env->NewStringUTF(sessionDescription.GetFmtp().c_str());
    jstring rtpMap = env->NewStringUTF(sessionDescription.GetRtpMap().c_str());
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jmethodID constructor = env->GetMethodID(sessionDescriptionClass, "<init>", "(Lco/instil/surge/client/SessionType;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    return env->NewObject(sessionDescriptionClass, constructor, type, controlUrl, formatParameters, rtpMap);
}

jobject convertSessionDescriptions(JNIEnv *env, std::vector<Surge::SessionDescription> sessionDescriptions) {
    jclass sessionDescriptionClass = env->FindClass("co/instil/surge/client/SessionDescription");
    jobjectArray jSessionDescriptions = env->NewObjectArray(sessionDescriptions.size(), sessionDescriptionClass, nullptr);
    for (int i = 0; i < sessionDescriptions.size(); i++) {
        env->SetObjectArrayElement(jSessionDescriptions, i, convertSessionDescription(env, sessionDescriptions[i]));
    }
    return jSessionDescriptions;
}

jobject convertDescribeResponse(JNIEnv *env, Surge::DescribeResponse *response) {
    jclass cls = env->FindClass("co/instil/surge/client/DescribeResponse");
    jmethodID  constructor = env->GetMethodID(cls, "<init>", "(ILjava/util/Map;Ljava/lang/String;[Lco/instil/surge/client/SessionDescription;)V");
    jint statusCode = response->GetCode();
    jobject headers = convertMap(env, response->GetHeaders());
    jstring body = convertString(env, response->GetBodyString());
    return env->NewObject(cls, constructor, statusCode, headers, body, convertSessionDescriptions(env, response->GetSessionDescriptions()));
}

/*
 * Converting from Java types to C++ types.
 */

std::string convertJString(JNIEnv *env, jstring jString) {
    const char * chars = env->GetStringUTFChars(jString, JNI_FALSE);
    std::string string(chars);
    env->ReleaseStringUTFChars(jString, chars);
    return string;
}

Surge::RtspSessionType convertJSessionType(JNIEnv *env, jobject jSessionDescription) {
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

Surge::SessionDescription convertJSessionDescription(JNIEnv *env, jobject jSessionDescription) {
    Surge::RtspSessionType type = convertJSessionType(env, jSessionDescription);
    std::string controlUrl = convertJString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getControlUrl"));
    std::string rtpMap = convertJString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getRtpMap"));
    std::string formatParameters = convertJString(env, callJSessionDescriptionGetter(env, jSessionDescription, "getFormatParameters"));
    Surge::SessionDescription sessionDescription(type, controlUrl, rtpMap, formatParameters, -1, -1, -1);
    return sessionDescription;
}

/*
 * Java native method implementations.
 */

//jint JNI_OnLoad(JavaVM *vm, void *reserved);

//void JNI_OnUnload(JavaVM *vm, void *reserved);

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_load(JNIEnv *, jclass) {
    static SurgeJni::RtspLoggingDelegateWrapper *delegate = new SurgeJni::RtspLoggingDelegateWrapper();
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingDelegate(delegate);
}

JNIEXPORT
jlong JNICALL Java_co_instil_surge_client_RtspClient_createNativeClientInstance(JNIEnv *env, jobject callingObject, jobject delegate) {
    static JavaVM *jvm;
    env->GetJavaVM(&jvm);
    Surge::RtspClient *client = new Surge::RtspClient(new SurgeJni::RtspClientDelegateWrapper(jvm, env->NewWeakGlobalRef(delegate)));
    return reinterpret_cast<jlong>(client);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_options__(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Options();
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_options__Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring url) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Options(convertJString(env, url));
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring url) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::DescribeResponse *response = client->Describe(convertJString(env, url));
    return convertDescribeResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring url, jstring user, jstring password) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::DescribeResponse *response = client->Describe(convertJString(env, url), convertJString(env, user), convertJString(env, password));
    return convertDescribeResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *env, jobject callingObject, jobject jSessionDescription) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Setup(convertJSessionDescription(env, jSessionDescription), false);
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_play(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Play(true);
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_pause(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Pause();
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_tearDown(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Teardown(true);
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_keepAlive(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->KeepAlive();
    return convertResponse(env, response);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_close(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeJni::RtspClientDelegateWrapper *delegate = (SurgeJni::RtspClientDelegateWrapper *)client->GetDelegate();
    client->StopClient();
    env->DeleteWeakGlobalRef(delegate->GetJavaDelegate());
    delete delegate;
    delete client;
}
