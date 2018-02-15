// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef RtpPacketBuffer_h
#define RtpPacketBuffer_h

#include <functional>
#include <deque>

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
            missedPackets += numMissedPackets;
            DEBUG("Packet loss detected. Lost: " << missedPackets << "; Successful: " << successfulPackets << "; Out of order: " << oooPackets << ". Loss rate: " << ((float)(missedPackets * 100) / (successfulPackets + oooPackets + missedPackets)) << "%");
        }
        
        void LogOutOfOrderPacket() {
            ++oooPackets;
            missedPackets = std::max(--missedPackets, 0);
            DEBUG("Out of order packet detected. Lost: " << missedPackets << "; Successful: " << successfulPackets << "; Out of order: " << oooPackets << ". Loss rate: " << ((float)(missedPackets * 100) / (successfulPackets + oooPackets + missedPackets)) << "%");
        }
        
    private:
        std::deque<RtpPacketBufferItem> buffer;
        
        int bufferDelayMilliseconds;
        
        std::function<void(RtpPacket *)> packetAvailableCallback;
        
        long calculatedSequenceNumber = -1;
        int successfulPackets = 0;
        int missedPackets = 0;
        int oooPackets = 0;
    };
}

#endif /* RtpPacketBuffer_h */
