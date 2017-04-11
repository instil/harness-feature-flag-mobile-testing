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

#include "RtspClientWrapper.h"
#include "RtspClientDelegateWrapper.h"
#include "RtspLoggingDelegateWrapper.h"
#include "TypeConverters.h"

using namespace SurgeJni::JavaTypeConverters;
using namespace SurgeJni::NativeTypeConverters;

SurgeJni::ClassLoader *classLoader;

/* Helper Functions */

void callResponseCallback(JNIEnv *env, jobject jResponseCallback, jobject jResponse) {
//    jclass cls = classLoader->getClass(jResponseCallback);
    jclass cls = classLoader->findClass("co/instil/surge/callbacks/ResponseCallback");
    jmethodID method = env->GetMethodID(cls, "response", "(Lco/instil/surge/client/Response;)V");
    env->CallVoidMethod(jResponseCallback, method, jResponse);
}

JNIEnv* getEnv(JavaVM *jvm) {
    JNIEnv *env;
    int status = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status < 0) {
        int status = jvm->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            return nullptr;
        }
    }

    return env;
}

/* Wrapper Implementation */

Surge::RtspClient* getClient(JNIEnv *env, jobject callingObject) {
    jclass callingObjectClass = env->GetObjectClass(callingObject);
    jfieldID field = env->GetFieldID(callingObjectClass, "nativeClient", "J");
    jlong clientHandle = env->GetLongField(callingObject, field);
    return reinterpret_cast<Surge::RtspClient*>(clientHandle);
}

//jint JNI_OnLoad(JavaVM *vm, void *reserved);
//void JNI_OnUnload(JavaVM *vm, void *reserved);

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_load(JNIEnv *, jclass) {
    static SurgeJni::RtspLoggingDelegateWrapper *delegate = new SurgeJni::RtspLoggingDelegateWrapper();
    SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
    logger.SetLoggingDelegate(delegate);
}

static JavaVM *jvm;
JNIEXPORT
jlong JNICALL Java_co_instil_surge_client_RtspClient_createNativeClientInstance(JNIEnv *env, jobject callingObject, jobject delegate) {
    classLoader = new SurgeJni::ClassLoader(env);

    env->GetJavaVM(&jvm);
    Surge::RtspClient *client = new Surge::RtspClient(new SurgeJni::RtspClientDelegateWrapper(jvm, env->NewWeakGlobalRef(delegate)));
    return reinterpret_cast<jlong>(client);
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_options__Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Options([=](Surge::RtspResponse *response) {
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_options__Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2 (JNIEnv *env, jobject callingObject, jstring url, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Options(convertString(env, url), [=](Surge::RtspResponse *response) {
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jstring url, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Describe(convertString(env, url), [=](Surge::DescribeResponse *response) {
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertDescribeResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2Lco_instil_surge_callbacks_ResponseCallback_2(JNIEnv *env, jobject callingObject, jstring url, jstring user, jstring password, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Describe(convertString(env, url), convertString(env, user), convertString(env, password), [=](Surge::DescribeResponse *response) {
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertDescribeResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
    });
}

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *env, jobject callingObject, jobject jSessionDescription, jobject callback) {
    Surge::RtspClient *client = getClient(env, callingObject);

    jobject globalCallback = env->NewGlobalRef(callback);

    client->Setup(convertSessionDescription(env, jSessionDescription), false, [=](Surge::SetupResponse *response) {
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
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
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
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
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
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
        JNIEnv *env = getEnv(jvm);

        callResponseCallback(env, globalCallback, convertResponse(classLoader, response));
        env->DeleteGlobalRef(globalCallback);
    });
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

JNIEXPORT
void JNICALL Java_co_instil_surge_client_RtspClient_stopStream(JNIEnv *env, jobject callingObject) {
    Surge::RtspClient *client = getClient(env, callingObject);
    client->StopStream();
}

