// -*-c++-*-
#ifndef __MJPEG_DEPACKETIZER_H__
#define __MJPEG_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"

#include <vector>

namespace Surge {

    class MJPEGDepacketizer {
    public:

        MJPEGDepacketizer(const SessionDescription* palette, const RtpPacket *packet, bool isFirstPayload);

        void AddToFrame(std::vector<unsigned char> * const frame);

    private:

        void ParseJpegHeader(const unsigned char * buffer);

        void ParseRestartMarkerHeader(const unsigned char * buffer);

        void ParseQuantizationHeader(const unsigned char * buffer);

        void ParseQuantizationTableData(const unsigned char * buffer);

        int m_height;
        int m_width;
        int m_type;
        int m_qValue;
        long m_fragmentOffset;
        int m_restartHeaderSize;
        int m_dri;
        int m_quantizationPayloadLength;
        unsigned char m_lumq[64];
	unsigned char m_chrq[64];
        
        const SessionDescription *m_palette;
        const RtpPacket *m_packet;

        std::vector<unsigned char> m_payload;
    };
    
};

#endif //__MJPEG_DEPACKETIZER_H__
