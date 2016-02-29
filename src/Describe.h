// -*-c++-*-
#ifndef __DESCRIBE_H__
#define __DESCRIBE_H__

#include "RtspCommand.h"

#include <cstdlib>
#include <string>

namespace Surge {

    class DescribeRequest: public RtspCommand {
    public:
        Describe(const std::string url,
                 int nextSequenceNumber,
                 bool isLive) : m_url(url),
                                m_sequenceNumber(nextSequenceNumber),
                                m_isLive(isLive) {
            
            string packet = "DESCRIBE " + url + " RTSP/1.0\r\n";
            char headerField[1024];
            snprintf(headerField, sizeof(headerField),
                     "CSeq: %d\r\n", nextSequenceNumber);
            packet += std::string(headerField);

            packet += "Content-Length: 0\r\n";
            packet += "Accept: application/sdp\r\n";

            if (isLive) {
                packet += "Range: npt=now-";
            } else {
                // TODO...
                
                
            }
            // range delimiter
            packet += "\r\n";
            
            packet += "Scale: 1\r\n";
            packet += "Speed: 1\r\n";

            if (RtspCommandFactory::IsBasicAuthHeaderAvailable()) {
                packet += RtspCommandFactory::GetBasicAuthHeader();
            }
            packet += "\r\n";

            m_buffer = (unsigned char *)malloc(packet.length());
            m_length = packet.copy((char *)m_buffer, packet.length(), 0);
        }

        ~Describe() {
            free(m_buffer);
        }
        
        const unsigned char * BytesPointer() const override {
            return m_buffer;
        }

        size_t PointerLength() const override {
            return m_length;
        }

    private:

        std::string m_url;
        int m_sequenceNumber;
        bool m_isLive;
        
        unsigned char *m_buffer;
        size_t m_length;
    }
    
};

#endif //__DESCRIBE_H__
