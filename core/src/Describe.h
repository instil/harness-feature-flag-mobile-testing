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

#ifndef __DESCRIBE_H__
#define __DESCRIBE_H__

#include "RtspCommand.h"

#include <cstdlib>
#include <string>

#include "DateTime.h"

namespace Surge {

    class DescribeRequest: public RtspCommand {
    public:
        DescribeRequest(const std::string& url,
                        int nextSequenceNumber,
                        const std::string& authHeader,
                        const SurgeUtil::DateTime& startTime) {
            
            std::string rangeHeader = "Range: clock=" + SurgeUtil::convertToISO(startTime) + "-";
            generateDescribe(url, nextSequenceNumber, authHeader, rangeHeader);
        }
        
        DescribeRequest(const std::string& url,
                        int nextSequenceNumber,
                        const std::string& authHeader) {
            
            std::string rangeHeader = kRangeHeaderForLiveStream;
            generateDescribe(url, nextSequenceNumber, authHeader, rangeHeader);
        }
    
        ~DescribeRequest() {
            free(m_buffer);
        }
        
        const unsigned char * BytesPointer() const override {
            return m_buffer;
        }

        size_t PointerLength() const override {
            return m_length;
        }

    private:
        const std::string kRangeHeaderForLiveStream = "Range: npt=now-";
        
        unsigned char *m_buffer;
        size_t m_length;
        
        void generateDescribe(const std::string& url,
                              int nextSequenceNumber,
                              const std::string& authHeader,
                              const std::string& rangeHeader) {
            
            std::string packet = "DESCRIBE " + url + " RTSP/1.0\r\n";
            
            char headerField[1024];
            snprintf(headerField, sizeof(headerField),
                     "CSeq: %d\r\n", nextSequenceNumber);
            
            packet += std::string(headerField);
            packet += "Accept: application/sdp\r\n";
            
            if (!rangeHeader.empty()) {
                packet += rangeHeader;
            }
            else {
                packet += kRangeHeaderForLiveStream;
            }
            
            // range delimiter
            packet += "\r\n";
            
            if (!authHeader.empty()) {
                packet += authHeader;
            }
            packet += "\r\n";
            
            m_buffer = (unsigned char *)malloc(packet.length());
            m_length = packet.copy((char *)m_buffer, packet.length(), 0);

        }
    };
    
}

#endif //__DESCRIBE_H__
