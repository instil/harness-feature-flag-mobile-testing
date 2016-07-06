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

#ifndef ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
#define ANDROID_RTSPCLIENTDELEGATEWRAPPER_H

#include <jni.h>
#include <Surge.h>

namespace SurgeJni {

    class RtspClientDelegateWrapper : public Surge::IRtspClientDelegate {

    public:
        RtspClientDelegateWrapper(JavaVM *jvm, jobject jDelegate) : jvm(jvm), jDelegate(jDelegate) {}

        void ClientDidTimeout();

        void StreamConfigChanged(bool wasRedirect);

        void ClientReceivedFrame(const unsigned char * buffer,
                                 size_t length,
                                 int32_t width,
                                 int32_t height,
                                 int32_t presentationTime,
                                 int32_t duration);

        jobject GetJavaDelegate() {
            return jDelegate;
        }

    private:
        JavaVM *jvm;
        jobject jDelegate;
    };

}

#endif //ANDROID_RTSPCLIENTDELEGATEWRAPPER_H
