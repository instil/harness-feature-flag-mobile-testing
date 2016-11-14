// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __RTSP_INTERLEAVED_TRANSPORT_H__
#define __RTSP_INTERLEAVED_TRANSPORT_H__

//#include "Transport.h"
#include "UvwTransport.h"

#include <cstdlib>
#include <vector>


namespace Surge {
    
    class InterleavedRtspTransport : public UvwTransport {
    public:
        InterleavedRtspTransport(ISocketHandlerDelegate * const delegate);

        ~InterleavedRtspTransport();

        bool IsInterleavedTransport() override { return true; };

        bool IsRtpTransportTCP() override { return true; };

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        void SetRtcpInterleavedChannel(int channel) { m_rtcpInterleavedChannel = channel; }

        std::string GetTransportHeaderString() const override {
            char rtp_channel[12];
            char rtcp_channel[12];

            memset(rtp_channel, 0, sizeof(rtp_channel));
            memset(rtcp_channel, 0, sizeof(rtcp_channel));
            snprintf(rtp_channel, sizeof(rtp_channel), "%d", m_rtpInterleavedChannel);
            snprintf(rtcp_channel, sizeof(rtcp_channel), "%d", m_rtcpInterleavedChannel);
            
            return "RTP/AVP/TCP;unicast;interleaved="
                + std::string(rtp_channel)
                + "-"
                + std::string(rtcp_channel);
        }
        
        void RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) {
            m_receivedBuffer.clear();
            UvwTransport::RtspTcpOpen(host, port, callback);
        }

    protected:
        void RtspHandleReceive(const char*, size_t) override;
        void RtspHandleReceive(const SurgeUtil::WaitableEvent& event) {};

    private:
        std::vector<unsigned char> m_receivedBuffer;
        size_t m_readBufferSize;
        
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;
    };
}

#endif //__RTSP_INTERLEAVED_TRANSPORT_H__
