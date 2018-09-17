//
// Created by Paul Shields on 14/09/2018.
//

#ifndef ANDROID_JAVAAUTHENTICATOR_H
#define ANDROID_JAVAAUTHENTICATOR_H

#include <jni.h>
#include <Surge.h>
#include <vector>

#include "ClassLoader.h"

namespace SurgeJni {

    class JavaAuthenticator : public Surge::BaseAuthenticator {
    public:
        JavaAuthenticator(jobject javaAuthenticator, ClassLoader *classLoader);
        ~JavaAuthenticator();

        std::vector<std::string> GenerateAuthHeadersFor(const std::string &url, const std::string &method, const std::string &username, const std::string &password) override;
        std::vector<char> FirstBytesOnTheWireAuthentication(const std::string &username, const std::string &password) override;

    private:
        jobject javaAuthenticator;
        ClassLoader *classLoader;
    };
}

#endif //ANDROID_JAVAAUTHENTICATOR_H
