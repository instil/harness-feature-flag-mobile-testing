// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef RtpPacketBuffer_h
#define RtpPacketBuffer_h

#include <functional>
#include <deque>
#include <vector>

#include "RtpPacket.h"
#include "Logging.h"

#define MAX_SEQ_NUM 65535
#define SEQ_NUM_ROLLOVER_THRESHOLD 1000

#define MIN_ROLLOVER_THRESHOLD SEQ_NUM_ROLLOVER_THRESHOLD
#define MAX_ROLLOVER_THRESHOLD MAX_SEQ_NUM-SEQ_NUM_ROLLOVER_THRESHOLD

namespace Surge {
    
    struct RtpPacketBufferItem {
        Surge::RtpPacket *packet;
        long long timestamp;
        
        RtpPacketBufferItem(RtpPacket *packet, long long timestamp) {
            this->packet = packet;
            this->timestamp = timestamp;
        }
    };

    struct RtpPacketBufferInfo {
        const long successfulPacketsCount;
        const long missedPacketsCount;
        const long oooPacketsCount;
        const std::vector<long> &sequenceNumbersOfLostPackets;

        RtpPacketBufferInfo(const int successfulPackets,
                            const int missedPackets,
                            const int oooPackets,
                            const std::vector<long> &sequenceNumbersOfLostPackets)
        : successfulPacketsCount(successfulPackets),
          missedPacketsCount(missedPackets),
          oooPacketsCount(oooPackets),
          sequenceNumbersOfLostPackets(sequenceNumbersOfLostPackets) { }
    };
    
    class RtpPacketBuffer {
        
    public:
        RtpPacketBuffer(int bufferDelayMilliseconds);
        RtpPacketBuffer() : RtpPacketBuffer(200) { }
        ~RtpPacketBuffer();
        
        void AddPacketToBuffer(RtpPacket *packet);
        
        void SetPacketAvailableCallback(std::function<void(Surge::RtpPacket *)> callback) {
            packetAvailableCallback = callback;
        }
        
        int GetBufferDelay() {
            return this->bufferDelayMilliseconds;
        }
        
        void SetBufferDelay(int bufferDelayMilliseconds) {
            INFO("Updated the packet buffer delay value to " << bufferDelayMilliseconds << " ms.");
            this->bufferDelayMilliseconds = bufferDelayMilliseconds;
        }

        RtpPacketBufferInfo GetDiagnosticsInfo() {
            return RtpPacketBufferInfo(successfulPackets, missedPackets, oooPackets, sequenceNumbersOfLostPackets);
        }
        
    private:
        void AddToBuffer(RtpPacket *packet);
        void PopPacketsPastDelayTime();
        
        bool PacketIsInSequentialOrder(const RtpPacket *packetToAdd, const RtpPacket *adjacentPacket);
        bool PacketNeedsRolledBack(const RtpPacket *packetToAdd, const RtpPacket *adjacentPacket);
        int NumberOfPacketsLost(const RtpPacket *packetToAdd, const std::deque<RtpPacketBufferItem>& buffer);
        
        void LogSuccessfulPacket() {
            ++successfulPackets;
        }
        
        void LogMissedPackets(const int numMissedPackets) {
            DEBUG("Packet loss detected. Lost: " << missedPackets << "; Successful: " << successfulPackets << "; Out of order: " << oooPackets << ". Loss rate: " << ((float)(missedPackets * 100) / (successfulPackets + oooPackets + missedPackets)) << "%");

            missedPackets += numMissedPackets;
            for (int i = 1; i <= numMissedPackets; i++) {
                sequenceNumbersOfLostPackets.push_back(buffer.back().packet->GetSequenceNumber() + i);
            }
        }
        
        void LogOutOfOrderPacket(const RtpPacket *packet) {
            DEBUG("Out of order packet detected. Lost: " << missedPackets << "; Successful: " << successfulPackets << "; Out of order: " << oooPackets << ". Loss rate: " << ((float)(missedPackets * 100) / (successfulPackets + oooPackets + missedPackets)) << "%");

            ++oooPackets;
            missedPackets = std::max(--missedPackets, 0);
            sequenceNumbersOfLostPackets.erase(std::remove(sequenceNumbersOfLostPackets.begin(),
                                                           sequenceNumbersOfLostPackets.end(),
                                                           packet->GetSequenceNumber()),
                                               sequenceNumbersOfLostPackets.end());
        }
        
    private:
        std::deque<RtpPacketBufferItem> buffer;
        
        int bufferDelayMilliseconds;
        
        std::function<void(RtpPacket *)> packetAvailableCallback;
        
        int successfulPackets = 0;
        int missedPackets = 0;
        int oooPackets = 0;

        std::vector<long> sequenceNumbersOfLostPackets;
    };
}

#endif /* RtpPacketBuffer_h */
