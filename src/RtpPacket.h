// -*-c++-*-
#ifndef __RTP_PACKET_H__
#define __RTP_PACKET_H__

#include "Logging.h"
#include "Config.h"

#include <cstdlib>

#include <sstream>
  using std::ostringstream;

#include <stdexcept>
  using std::runtime_error;

namespace Surge {

    struct _RTPHeader {
#if IS_BIG_ENDIAN
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t extension:1;
	uint8_t csrccount:4;
	
	uint8_t marker:1;
	uint8_t payloadtype:7;
#else // little endian
	uint8_t csrccount:4;
	uint8_t extension:1;
	uint8_t padding:1;
	uint8_t version:2;
	
	uint8_t payloadtype:7;
	uint8_t marker:1;
#endif // RTP_BIG_ENDIAN
	
	uint16_t sequencenumber;
	uint32_t timestamp;
	uint32_t ssrc;
    } __attribute__((packed)) ;

    struct _RTPExtensionHeader {
	uint16_t extid;
	uint16_t length;
    } __attribute__((packed)) ;

    class RtpPacket {
    public:
        RtpPacket(const unsigned char *buffer, size_t length): m_timestmap(0),
                                                               m_extensionID(-1),
                                                               m_extension(nullptr),
                                                               m_extensionLength(0),
                                                               m_payload(nullptr),
                                                               m_payloadLength(0) {
            
            memcpy(&m_header, buffer, sizeof(struct _RTPHeader));

            m_version = static_cast<int>(m_header.version);
            m_sequenceNumber = ntohs(m_header.sequencenumber);
            m_type = static_cast<int>(m_header.payloadtype);
            m_marker = static_cast<bool>(m_header.marker);

            if (m_version != 2) {
                ostringstream message;
                message << "Invalid RTP Packet Version: " << m_version;
                throw runtime_error{ message.str() };
            }

            size_t padding_bytes = 0;
            if (m_header.padding) {
                padding_bytes = static_cast<int>(buffer[length - 1]);
                if (padding_bytes <= 0) {
                    ostringstream message;
                    message << "Unhandled RTP Packet Padding: " << m_header.padding;
                    throw runtime_error{ message.str() };
                }
            }
            
            size_t payload_offset = sizeof(struct _RTPHeader) + (m_header.csrccount * sizeof(uint32_t));
            
            bool has_extension = (bool)m_header.extension;
            if (has_extension) {
                size_t extension_header_offset = payload_offset;
                const struct _RTPExtensionHeader* extension_header = (struct _RTPExtensionHeader*)
                    buffer + extension_header_offset;

                m_extensionID = static_cast<int>(ntohs(extension_header->extid));
                m_extensionLength = static_cast<size_t>(ntohs(extension_header->length));

                m_extension = (unsigned char*)malloc(m_extensionLength);
                memset(m_extension, 0, m_extensionLength);

                const unsigned char *extension_data_pointer = buffer
                    + payload_offset
                    + sizeof(struct _RTPExtensionHeader);
                memcpy(m_extension, extension_data_pointer, m_extensionLength);

                payload_offset += sizeof(struct _RTPExtensionHeader) + m_extensionLength;
            }

            m_timestmap = ntohl(m_header.timestamp);
            
            m_payloadLength = length - padding_bytes - payload_offset;
            m_payload = (unsigned char *)malloc(m_payloadLength);
            memset(m_payload, 0, m_payloadLength);
            memcpy(m_payload, buffer + payload_offset, m_payloadLength);
        }

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
        
        struct _RTPHeader m_header;
    };
    
};

#endif // __RTP_PACKET_H__
