#include "Config.h"
#include "RtpPacket.h"
#include "Logging.h"

#include <sstream>
  using std::ostringstream;

#include <stdexcept>
  using std::runtime_error;

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

Surge::RtpPacket::RtpPacket(const unsigned char *buffer, size_t length): m_timestmap(0),
                                                                         m_extensionID(-1),
                                                                         m_extension(nullptr),
                                                                         m_extensionLength(0),
                                                                         m_payload(nullptr),
                                                                         m_payloadLength(0) {
    struct _RTPHeader header;
    memcpy(&header, buffer, sizeof(struct _RTPHeader));

    m_version = static_cast<int>(header.version);
    m_sequenceNumber = ntohs(header.sequencenumber);
    m_type = static_cast<int>(header.payloadtype);
    m_marker = static_cast<bool>(header.marker);

    if (m_version != 2) {
        ostringstream message;
        message << "Invalid RTP Packet Version: " << m_version;
        throw runtime_error{ message.str() };
    }

    size_t padding_bytes = 0;
    if (header.padding) {
        INFO("RTP PACKET HAS PADDING");
        padding_bytes = static_cast<int>(buffer[length - 1]);
        if (padding_bytes <= 0) {
            ostringstream message;
            message << "Unhandled RTP Packet Padding: " << header.padding;
            throw runtime_error{ message.str() };
        }
    }
            
    size_t payload_offset = sizeof(struct _RTPHeader) + (header.csrccount * sizeof(uint32_t));
            
    bool has_extension = (bool)header.extension;
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

    m_timestmap = ntohl(header.timestamp);
            
    m_payloadLength = length - padding_bytes - payload_offset;
    
    m_payload = (unsigned char *)malloc(m_payloadLength);
    memcpy(m_payload, buffer + payload_offset, m_payloadLength);

    /*INFO("Version: " << m_version <<
         " - Sequence: " << m_sequenceNumber <<
         " - Type: " << m_type <<
         " - Marker: " << m_marker <<
         " - Timestamp: " << m_timestmap <<
         " - Padding: " << padding_bytes <<
         " - Extension: " << has_extension <<
         " - Payload Offset: " << payload_offset <<
         " - Payload Length: " << m_payloadLength <<
         " - Total Packet Length: " << length);*/
}
