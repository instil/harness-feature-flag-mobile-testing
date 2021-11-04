// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspClientWrapper.h"
#include "RtspClientDelegateWrapper.h"
#include "RtspLoggingDelegateWrapper.h"
#include "TypeConverters.h"
#include "JavaAuthenticator.h"

using namespace SurgeJni::JavaTypeConverters;
using namespace SurgeJni::NativeTypeConverters;

SurgeJni::ClassLoader *classLoader;

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
jlong JNICALL Java_co_instil_surge_client_RtspClient_createNativeClientInstance(JNIEnv *env,
                                                                                jobject callingObject,
                                                                                jobject delegate) {
    JavaVM *jvm;
    env->GetJavaVM(&jvm);

    Surge::RtspClient *client = new Surge::RtspClient(new SurgeJni::RtspClientDelegateWrapper(jvm, env->NewWeakGlobalRef(delegate)));
    return reinterpret_cast<jlong>(client);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_connect(JNIEnv *env, jobject callingObject, jstring url, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Connect(convertString(env, url), [=](bool result) {
        JNIEnv *env = classLoader->AttachToJvm();

        callBooleanCallback(classLoader, globalCallback, result);

        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_disconnect(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->Disconnect();
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setCredentials(JNIEnv *env, jobject callingObject, jstring username, jstring password) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetCredentials(convertString(env, username), convertString(env, password));
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_describe(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Describe([=](Surge::DescribeResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertDescribeResponse(classLoader, response));
        delete response;

        if (globalCallback != NULL) {
            env->DeleteGlobalRef(globalCallback);
        }

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *env, jobject callingObject, jobject jSessionDescription, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Setup(convertSessionDescription(env, jSessionDescription), [=](Surge::SetupResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;

        if (globalCallback != NULL) {
            env->DeleteGlobalRef(globalCallback);
        }

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_play__(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);

    client->Play([&](Surge::RtspResponse *response) { delete response; });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_play__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Play([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;
        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_pause__(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);

    client->Pause([=](Surge::RtspResponse *response) {
        delete response;
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_pause__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Pause([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;
        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}
JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_options__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Options([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;
        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_tearDown__(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);

    client->Teardown([](Surge::RtspResponse *response) {
        delete response;
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_tearDown__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Teardown([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;
        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_keepAlive(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->KeepAlive([=](Surge::RtspResponse *response) {
        JNIEnv *env = classLoader->AttachToJvm();

        callResponseCallback(classLoader, globalCallback, convertResponse(classLoader, response));
        delete response;
        env->DeleteGlobalRef(globalCallback);

        classLoader->DetachFromJvm();
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setTimeRange(JNIEnv *env, jobject callingObject, jobject startTime, jobject endTime) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetTimeRange(convertDate(env, startTime), convertDate(env, endTime));
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setStartTime(JNIEnv *env, jobject callingObject, jobject startTime) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetStartTime(convertDate(env, startTime));
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setEndTime(JNIEnv *env, jobject callingObject, jobject endTime) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetEndTime(convertDate(env, endTime));
}

JNIEXPORT
jboolean JNICALL Java_co_instil_surge_client_RtspClient_isInterleavedTransport (JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    return (jboolean)client->IsInterleavedTransport();
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setInterleavedTransport (JNIEnv *env, jobject callingObject, jboolean isEnabled) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetInterleavedTransport(isEnabled);
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

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setTLSCertificateValidationEnabled(JNIEnv *env, jobject callingObject, jboolean isEnabled) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetTLSCertificateValidationEnabled(isEnabled);
}

JNIEXPORT
jboolean JNICALL Java_co_instil_surge_client_RtspClient_isTLSCertificateValidationEnabled(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    return static_cast<jboolean>(client->IsTLSCertificateValidationEnabled());
}

JNIEXPORT void JNICALL Java_co_instil_surge_client_RtspClient_setTLSSelfSignedCertsAllowed(JNIEnv *env, jobject callingObject, jboolean isEnabled) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetTLSSelfSignedCertsAllowed(isEnabled);
}

JNIEXPORT
jboolean JNICALL Java_co_instil_surge_client_RtspClient_tlsSelfSignedCertsAllowed(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    return static_cast<jboolean>(client->TLSSelfSignedCertsAllowed());
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setTLSTrustedCertificate(JNIEnv *env, jobject callingObject, jstring certificate) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->SetTLSTrustedCertificate(convertString(env, certificate));
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_addAuthenticator(JNIEnv *env, jobject callingObject, jobject jAuthenticator) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeJni::JavaAuthenticator *authenticator = new SurgeJni::JavaAuthenticator(jAuthenticator, classLoader);
    client->AddAuthenticator(authenticator);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_removeAuthenticator__Lco_instil_surge_authentication_SurgeAuthenticator_2(JNIEnv *env, jobject callingObject, jobject jAuthenticator) {
    // TODO!
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_removeAuthenticator__I(JNIEnv *env, jobject callingObject, jint authenticatorId) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->RemoveAuthenticator((unsigned int)authenticatorId);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_getDiagnosticsOnPacketLoss(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtpPacketBufferInfo packetBufferInfo = client->GetDiagnosticsOnPacketLoss();
    return convertRtpPacketBufferInfo(classLoader, packetBufferInfo);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_close(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    SurgeJni::RtspClientDelegateWrapper *delegate = (SurgeJni::RtspClientDelegateWrapper *)client->GetDelegate();

    client->Disconnect();

    if (delegate->GetJavaDelegate() != NULL) {
        env->DeleteWeakGlobalRef(delegate->GetJavaDelegate());
    }
    delete delegate;
    delete client;
}
