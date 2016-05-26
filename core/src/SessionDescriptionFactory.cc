// -*-c++-*-
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

#include "SessionDescriptionFactory.h"
#include "SessionDescriptionV0.h"

#include "Helpers.h"
#include "Logging.h"


namespace Surge {

    namespace SessionDescriptionFactory {

        std::vector<SessionDescription>* ParseSessionDescriptionsFromBuffer(const RawSessionDescription& description) {
            std::vector<SessionDescription> *sessionDescriptions = new std::vector<SessionDescription>;
            const std::vector<std::string> lines = description.GetLines();
            
            if (lines.size() > 0) {
                std::string version_line = lines[0];

                if (version_line.find("v=0") != std::string::npos) {
                    SessionDescriptionV0 sessionDescription(description);
                    sessionDescriptions->push_back(sessionDescription);
                }
                else if (!version_line.empty()) {
                    ERROR("Unhandled Session Description version: [" << version_line << "]");
                }
                else {
                    ERROR("No Session Description available");
                }
            }
            
            return sessionDescriptions;
        }
        
    }
}