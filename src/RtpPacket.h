// -*-c++-*-
#ifndef __RTP_PACKET_H__
#define __RTP_PACKET_H__

#include <cstdlib>

namespace Surge {

    class RtpPacket {
    public:
        RtpPacket(const unsigned char *buffer, uint16_t length);

        ~RtpPacket() {
            if (HasExtension()) {
                free(m_extension);
            }
            free(m_payload);
        }

        int GetVersion() const { return m_version; }

        int GetSequenceNumber() const { return m_sequenceNumber; }

        int GetType() const { return m_type; }

        bool IsMarked() const { return m_marker; }

        bool HasExtension() const { return m_extension != nullptr; }

        int GetExtensionID() const { return m_extensionID; }

        const unsigned char * GetExtensionData() const { return m_extension; }

        size_t GetExtensionLength() const { return m_extensionLength; }

        bool HasPayload() const { return m_payload != nullptr; }

        const unsigned char * PayloadData() const { return m_payload; }

        size_t PayloadLength() const { return m_payloadLength; }

        uint32_t GetTimestamp() const { return m_timestmap; }

    private:
        uint32_t m_timestmap;
        int m_version;
        int m_sequenceNumber;
        int m_type;
        bool m_marker;
        int m_extensionID;
        unsigned char *m_extension;
        size_t m_extensionLength;
        unsigned char *m_payload;
        size_t m_payloadLength;
    };
    
};

#endif // __RTP_PACKET_H__
