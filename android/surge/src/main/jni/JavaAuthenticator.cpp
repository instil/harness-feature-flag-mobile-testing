//
// Created by Paul Shields on 14/09/2018.
//

#include "JavaAuthenticator.h"
#include "TypeConverters.h"


SurgeJni::JavaAuthenticator::JavaAuthenticator(jobject javaAuthenticator, ClassLoader *classLoader) : classLoader(classLoader) {
    JNIEnv *env = classLoader->AttachToJvm();
    this->javaAuthenticator = env->NewGlobalRef(javaAuthenticator);
    classLoader->DetachFromJvm();
}

SurgeJni::JavaAuthenticator::~JavaAuthenticator() {
    JNIEnv *env = classLoader->AttachToJvm();
    env->DeleteGlobalRef(javaAuthenticator);
    classLoader->DetachFromJvm();
}

std::vector<std::string> SurgeJni::JavaAuthenticator::GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) {
    std::vector<std::string> result;
    JNIEnv *env = classLoader->AttachToJvm();

    jstring jUsername = NativeTypeConverters::convertString(env, username);
    jstring jPassword = NativeTypeConverters::convertString(env, password);

    jclass javaAuthenticatorClass = classLoader->findClass("co/instil/surge/authentication/SurgeAuthenticator");
    jmethodID generateAuthHeadersForMethod = env->GetMethodID(javaAuthenticatorClass, "generateAuthHeadersFor", "(Ljava/lang/String;Ljava/lang/String;)Ljava/util/List;");
    jobject listOfStrings = (jobject)env->CallObjectMethod(javaAuthenticator, generateAuthHeadersForMethod, jUsername, jPassword);

    if (listOfStrings != NULL) {
        result = JavaTypeConverters::convertList<std::string>(classLoader, listOfStrings, [env](jobject item) {
            return JavaTypeConverters::convertString(env, (jstring)item);
        });
    }

    classLoader->DetachFromJvm();

    return result;
}

std::vector<char> SurgeJni::JavaAuthenticator::FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
    std::vector<char> result;

    JNIEnv *env = classLoader->AttachToJvm();

    jstring jUsername = NativeTypeConverters::convertString(env, username);
    jstring jPassword = NativeTypeConverters::convertString(env, password);

    jclass javaAuthenticatorClass = classLoader->findClass("co/instil/surge/authentication/SurgeAuthenticator");
    jmethodID firstBytesOnTheWireAuthenticationMethod = env->GetMethodID(javaAuthenticatorClass, "firstBytesOnTheWireAuthentication", "(Ljava/lang/String;Ljava/lang/String;)[B");
    jbyteArray payload = (jbyteArray)env->CallObjectMethod(javaAuthenticator, firstBytesOnTheWireAuthenticationMethod, jUsername, jPassword);

    if (payload != NULL) {
        result = JavaTypeConverters::convertByteArrayToVector(classLoader, payload);
    }

    classLoader->DetachFromJvm();

    return result;
}