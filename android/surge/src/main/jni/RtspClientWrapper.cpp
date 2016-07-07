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
    Surge::RtspResponse *response = client->Options(convertString(env, url));
    return convertResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring url) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::DescribeResponse *response = client->Describe(convertString(env, url));
    return convertDescribeResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_describe__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2(JNIEnv *env, jobject callingObject, jstring url, jstring user, jstring password) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::DescribeResponse *response = client->Describe(convertString(env, url), convertString(env, user), convertString(env, password));
    return convertDescribeResponse(env, response);
}

JNIEXPORT
jobject JNICALL Java_co_instil_surge_client_RtspClient_setup(JNIEnv *env, jobject callingObject, jobject jSessionDescription) {
    Surge::RtspClient *client = getClient(env, callingObject);
    Surge::RtspResponse *response = client->Setup(convertSessionDescription(env, jSessionDescription), false);
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
