// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspClientWrapper.h"
#include "RtspClientDelegateWrapper.h"
#include "RtspLoggingDelegateWrapper.h"
#include "TypeConverters.h"

using namespace SurgeJni::JavaTypeConverters;
using namespace SurgeJni::NativeTypeConverters;

SurgeJni::ClassLoader *classLoader;

// JNI has issues at times accessing a local Java-land object on a different thread than it was created
// This is a quick fix, making the local objects global until a fix accessing global variables on a
// different thread via the RTSP response callbacks is found.
jobject currentCallbackObject;

void pushCallbackObject(jobject callbackObject) {
    currentCallbackObject = callbackObject;
}

jobject popCallbackObject() {
    return currentCallbackObject;
}

Surge::RtspClient* getClient(JNIEnv *env, jobject callingObject) {
    jclass callingObjectClass = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(callingObjectClass, "nativeClient", "J");
    jlong clientHandle = env->GetLongField(callingObject, field);
    return reinterpret_cast<Surge::RtspClient*>(clientHandle);
}

//jint JNI_OnLoad(JavaVM *vm, void *reserved);
//void JNI_OnUnload(JavaVM *vm, void *reserved);

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_load(JNIEnv *env, jclass callingClass) {
    classLoader = new SurgeJni::ClassLoader(env);

    static SurgeJni::RtspLoggingDelegateWrapper *delegate = new SurgeJni::RtspLoggingDelegateWrapper();
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingDelegate(delegate);
}

JNIEXPORT
JNIEXPORT jlong JNICALL Java_co_instil_surge_client_RtspClient_createNativeClientInstance(JNIEnv *env,
                                                                                          jobject callingObject,
                                                                                          jobject delegate,
                                                                                          jboolean isInterleavedTcpTransport) {
    JavaVM *jvm;
    env->GetJavaVM(&jvm);

    Surge::RtspClient *client = new Surge::RtspClient(new SurgeJni::RtspClientDelegateWrapper(jvm, env->NewWeakGlobalRef(delegate)), isInterleavedTcpTransport);
    return reinterpret_cast<jlong>(client);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_options__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Options([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_options__Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2 (JNIEnv *env, jobject callingObject, jstring url, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Options(convertString(env, url), [=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jstring url, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Describe(convertString(env, url), [=](Surge::DescribeResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertDescribeResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jstring url, jstring user, jstring password, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    pushCallbackObject(env->NewGlobalRef(callback));

    client->Describe(convertString(env, url), convertString(env, user), convertString(env, password), [=](Surge::DescribeResponse *response) {
        JNIEnv *env = classLoader->getEnv();
        jobject callbackObject = popCallbackObject();

        callResponseCallback(classLoader, callbackObject, convertDescribeResponse(classLoader, response));

        if (currentCallbackObject != NULL) {
            env->DeleteGlobalRef(callbackObject);
        }

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *env, jobject callingObject, jobject jSessionDescription, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    pushCallbackObject(env->NewGlobalRef(callback));

    client->Setup(convertSessionDescription(env, jSessionDescription), false, [=](Surge::SetupResponse *response) {
        JNIEnv *env = classLoader->getEnv();
        jobject callbackObject = popCallbackObject();

        callResponseCallback(classLoader, callbackObject, convertResponse(classLoader, response));

        if (callbackObject != NULL) {
            env->DeleteGlobalRef(callbackObject);
        }

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_play__ (JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);

    client->Play(true, [&](Surge::RtspResponse *response) { });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_play__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Play(true, [&](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_pause__(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);

    client->Pause([=](Surge::RtspResponse *response) {

    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_pause__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Pause([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setStartTime(JNIEnv *env, jobject callingObject, jobject jStartDate) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeUtil::DateTime startDate = convertDate(env, jStartDate);
    client->SetStartTime(startDate);
}

JNIEXPORT void JNICALL Java_co_instil_surge_client_RtspClient_setTimeRange (JNIEnv *env, jobject callingObject, jobject jStartDate, jobject jEndDate) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeUtil::DateTime startDate = convertDate(env, jStartDate);
    SurgeUtil::DateTime endDate = convertDate(env, jEndDate);
    client->SetTimeRange(startDate, endDate);
}


JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setEndTime(JNIEnv *env, jobject callingObject, jobject jEndDate) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeUtil::DateTime endDate = convertDate(env, jEndDate);
    client->SetEndTime(endDate);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_tearDown(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->StopStream();
    client->StopClient();
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_keepAlive(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->KeepAlive([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->getEnv();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);

        classLoader->detatchJniEnv();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_close(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeJni::RtspClientDelegateWrapper *delegate = (SurgeJni::RtspClientDelegateWrapper *)client->GetDelegate();

    client->StopClient();

    if (delegate->GetJavaDelegate() != NULL) {
        env->DeleteWeakGlobalRef(delegate->GetJavaDelegate());
    }
    delete delegate;
    delete client;
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_stopStream(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->StopStream();
}

JNIEXPORT
jboolean JNICALL Java_co_instil_surge_client_RtspClient_isInterleavedTransport (JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    return (jboolean)client->IsInterleavedTransport();
}

JNIEXPORT
jint JNICALL Java_co_instil_surge_client_RtspClient_getPacketBufferDelay(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    return (jint)client->GetPacketBufferDelay();
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setPacketBufferDelay(JNIEnv *env, jobject callingObject, jint packetBufferDelay) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetPacketBufferDelay(packetBufferDelay);
}