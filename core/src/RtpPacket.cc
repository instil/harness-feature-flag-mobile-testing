// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#define BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#ifdef __ANDROID__
    #include <endian.h>
#endif

#include <netinet/in.h>
#include "RtpPacket.h"
#include "Logging.h"

using std::ostringstream;

using std::runtime_error;

struct _RTPHeader {
#if BIG_ENDIAN == true
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

Surge::RtpPacket::RtpPacket(const unsigned char *buffer, uint16_t length): m_timestmap(0),
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
    m_marker = header.marker == 0 ? false : true;
    m_timestmap = ntohl(header.timestamp);

    if (m_version != 2) {
        ostringstream message;
        message << "Invalid RTP packet version: " << m_version;
        throw runtime_error{ message.str() };
    }

    uint16_t payload_offset = sizeof(struct _RTPHeader) + (header.csrccount * sizeof(uint32_t));

    bool has_extension = (header.extension == 0) ? false : true;
    if (has_extension) {
        size_t extension_header_offset = payload_offset;
        struct _RTPExtensionHeader extension_header;
        memset(&extension_header, 0, sizeof(struct _RTPExtensionHeader));
        memcpy(&extension_header, buffer + extension_header_offset, sizeof(struct _RTPExtensionHeader));
        
        m_extensionID = ntohs(extension_header.extid);
        m_extensionLength = (sizeof(uint32_t) * ntohs(extension_header.length));

        m_extension = (unsigned char*)malloc(m_extensionLength);
        memset(m_extension, 0, m_extensionLength);
        
        const unsigned char *extension_data_pointer = buffer
            + payload_offset
            + sizeof(struct _RTPExtensionHeader);
        memcpy(m_extension, extension_data_pointer, m_extensionLength);
        
        payload_offset += sizeof(struct _RTPExtensionHeader) + m_extensionLength;
    }
    
    m_payloadLength = length - payload_offset;
    
    m_payload = (unsigned char *)malloc(m_payloadLength);
    if (!m_payload) {
        FATAL("Unable to allocate rtp-packet payload.");
        ERROR("SEQ NUM: " << m_sequenceNumber <<
              " - VERSION: " << m_version <<
              " - PAYLOAD_LENGTH: " << m_payloadLength <<
              " - OFFSET: " << payload_offset <<
              " - TOTAL: " << length);
    } else {
        memcpy(m_payload, buffer + payload_offset, m_payloadLength);
    }
}
