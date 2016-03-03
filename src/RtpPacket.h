// -*-c++-*-
#ifndef __RTP_PACKET_H__
#define __RTP_PACKET_H__

#include "Logging.h"

namespace Surge {

    class RtpPacket {
    public:
        RtpPacket(const unsigned char *buffer, size_t length) {

            uint16_t version_network_order;
            memcpy(&version_network_order, buffer, 2);
            m_version = ntohs(version_network_order);
            INFO("OLD VERSION = " << version_network_order);
            INFO("VERSION = " << m_version);

            
            
        }

        ~RtpPacket() {
            
        }

    private:
        uint16_t m_version;

    };
    
};

#endif // __RTP_PACKET_H__
