// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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
