// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __MJPEG_DEPACKETIZER_H__
#define __MJPEG_DEPACKETIZER_H__

#include "SessionDescription.h"
#include "RtpPacket.h"
#include "Depacketizer.h"

#include <vector>

namespace Surge {

    class MJPEGDepacketizer : public Depacketizer {
    public:
        MJPEGDepacketizer(const SessionDescription sessionDescription,
                          std::vector<unsigned char> *frameBuffer)

                : Depacketizer(frameBuffer),
                  sessionDescription(sessionDescription) {}

        void ProcessPacket(const RtpPacket *packet, const bool isFirstPayload);

    private:
        
        size_t ProcessPacketHeaders(const unsigned char *payloadData, const size_t payloadLength);
        
        int ParseRestartMarkerHeader(const unsigned char *buffer);

        int ParseQuantizationTableLength(const unsigned char *buffer);
        
        void ParseQuantizationTable(const unsigned char *quantizationTableData,
                                    int quantizationTableLength,
                                    int qValue,
                                    unsigned char *lumq,
                                    unsigned char *chrq);

        const SessionDescription sessionDescription;
    };
    
};

#endif //__MJPEG_DEPACKETIZER_H__
