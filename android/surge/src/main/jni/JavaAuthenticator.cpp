//
// Created by Paul Shields on 14/09/2018.
//

#include "JavaAuthenticator.h"
#include "TypeConverters.h"


SurgeJni::JavaAuthenticator::JavaAuthenticator(jobject javaAuthenticator, ClassLoader *classLoader) : classLoader(classLoader) {
    JNIEnv *env = classLoader->getEnv();
    this->javaAuthenticator = env->NewGlobalRef(javaAuthenticator);
    classLoader->detatchJniEnv();
}

SurgeJni::JavaAuthenticator::~JavaAuthenticator() {
    JNIEnv *env = classLoader->getEnv();
    env->DeleteGlobalRef(javaAuthenticator);
    classLoader->detatchJniEnv();
}

std::vector<std::string> SurgeJni::JavaAuthenticator::GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) {
    JNIEnv *env = classLoader->getEnv();

    jstring jUsername = NativeTypeConverters::convertString(env, username);
    jstring jPassword = NativeTypeConverters::convertString(env, password);

    jclass javaAuthenticatorClass = classLoader->findClass("co/instil/surge/authentication/SurgeAuthenticator");
    jmethodID generateAuthHeadersForMethod = env->GetMethodID(javaAuthenticatorClass, "generateAuthHeadersFor", "(Ljava/lang/String;Ljava/lang/String;)Ljava/util/List;");
    jobject listOfStrings = (jobject)env->CallObjectMethod(javaAuthenticator, generateAuthHeadersForMethod, jUsername, jPassword);

    std::vector<std::string> result = JavaTypeConverters::convertList<std::string>(classLoader, listOfStrings, [env](jobject item) {
        return JavaTypeConverters::convertString(env, (jstring)item);
    });

    classLoader->detatchJniEnv();

    return result;
}

std::vector<char> SurgeJni::JavaAuthenticator::FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) {
    JNIEnv *env = classLoader->getEnv();

    jstring jUsername = NativeTypeConverters::convertString(env, username);
    jstring jPassword = NativeTypeConverters::convertString(env, password);

    jclass javaAuthenticatorClass = classLoader->findClass("co/instil/surge/authentication/SurgeAuthenticator");
    jmethodID firstBytesOnTheWireAuthenticationMethod = env->GetMethodID(javaAuthenticatorClass, "firstBytesOnTheWireAuthentication", "(Ljava/lang/String;Ljava/lang/String;)[B");
    jbyteArray payload = (jbyteArray)env->CallObjectMethod(javaAuthenticator, firstBytesOnTheWireAuthenticationMethod, jUsername, jPassword);

    std::vector<char> result = JavaTypeConverters::convertByteArrayToVector(classLoader, payload);

    classLoader->detatchJniEnv();

    return result;
}