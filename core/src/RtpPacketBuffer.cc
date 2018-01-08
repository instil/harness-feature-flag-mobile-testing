// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtpPacketBuffer.h"

#include "DateTime.h"

Surge::RtpPacketBuffer::RtpPacketBuffer(int bufferDelayMilliseconds) : bufferDelayMilliseconds(bufferDelayMilliseconds), packetAvailableCallback(nullptr) { }

Surge::RtpPacketBuffer::~RtpPacketBuffer() { }

void Surge::RtpPacketBuffer::AddPacketToBuffer(RtpPacket *packet) {
    AddToBuffer(packet);
    PopPacketsPastDelayTime();
}

void Surge::RtpPacketBuffer::AddToBuffer(RtpPacket *packet) {
    if (buffer.size() == 0 ||
        (PacketIsInSequentialOrder(packet, buffer.back().packet) &&
        !PacketNeedsRolledBack(packet, buffer.back().packet))) {

        LogSuccessfulPacket();
        int numPacketsLost = NumberOfPacketsLost(packet, buffer);
        if (numPacketsLost > 0) {
            LogMissedPackets(numPacketsLost);
        }

        RtpPacketBufferItem item(packet, SurgeUtil::currentTimeMilliseconds() + bufferDelayMilliseconds);
        buffer.push_back(item);
        return;
    }

    LogOutOfOrderPacket();

    std::deque<RtpPacketBufferItem>::reverse_iterator it = ++buffer.rbegin();

    while (it != buffer.rend()) {
        if (PacketNeedsRolledBack(packet, it->packet)) {
            ++it;
            continue;
        } else if (PacketIsInSequentialOrder(packet, it->packet)) {
            Surge::RtpPacketBufferItem item(packet, it->timestamp);
            buffer.insert(it.base(), item);
            return;
        }

        ++it;
    }

    // Packet is too late, frame already sent to decoder. Delete packet.
    DEBUG("Packet " << packet->GetSequenceNumber() << " arrived too late, frame already sent to decoder, deleting.");
    delete packet;
}

bool Surge::RtpPacketBuffer::PacketIsInSequentialOrder(const RtpPacket *packetToAdd, const RtpPacket *adjacentPacket) {
    return packetToAdd->GetSequenceNumber() > adjacentPacket->GetSequenceNumber() ||
          (packetToAdd->GetSequenceNumber() < MIN_ROLLOVER_THRESHOLD && adjacentPacket->GetSequenceNumber() > MAX_ROLLOVER_THRESHOLD);
}
                
bool Surge::RtpPacketBuffer::PacketNeedsRolledBack(const RtpPacket *packetToAdd, const RtpPacket *adjacentPacket) {
    return adjacentPacket->GetSequenceNumber() < MIN_ROLLOVER_THRESHOLD && packetToAdd->GetSequenceNumber() > MAX_ROLLOVER_THRESHOLD;
}

int Surge::RtpPacketBuffer::NumberOfPacketsLost(const RtpPacket *packet, const std::deque<RtpPacketBufferItem>& buffer) {
    
    if (buffer.size() == 0) {
        return 0;
    }

    RtpPacket *endOfBufferPacket = buffer.back().packet;

    if (packet->GetSequenceNumber() < MIN_ROLLOVER_THRESHOLD &&
        endOfBufferPacket->GetSequenceNumber() > MAX_ROLLOVER_THRESHOLD) {
        return (int)((MAX_SEQ_NUM - endOfBufferPacket->GetSequenceNumber()) + packet->GetSequenceNumber());
    }

    return (int)(packet->GetSequenceNumber() - endOfBufferPacket->GetSequenceNumber() - 1);
}

void Surge::RtpPacketBuffer::PopPacketsPastDelayTime() {
    long long bufferReleaseTime = SurgeUtil::currentTimeMilliseconds();
    while (buffer.size() > 0) {
        if (bufferReleaseTime < buffer.front().timestamp) {
            break;
        }
        
        if (packetAvailableCallback != nullptr) {
            packetAvailableCallback(buffer.front().packet);
        }
        
        buffer.pop_front();
    }
}
