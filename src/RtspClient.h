// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "StoppableThread.h"
#include "SocketHandler.h"
#include "SessionDescription.h"

#include "DescribeResponse.h"
#include "SetupResponse.h"

#include <string>

namespace Surge {

    class RtspClient : private SurgeUtil::Runnable {
    public:
        RtspClient();

        ~RtspClient();

        DescribeResponse* Describe(const std::string url,
                                   bool requires_auth,
                                   const std::string user,
                                   const std::string password);

        SetupResponse* Setup(const SessionDescription sessionDescription);

        RtspResponse* Play();

        RtspResponse* Pause();

        RtspResponse* Options();

        RtspResponse* Teardown();

        RtspResponse* KeepAlive();

        void StopClient();

    private:
        void Run() override;

        void StartSession();

        void ProcessRtpPacket(const RtpPacket* packet);
        
        int GetNextSequenceNumber() { return m_sequenceNumber++; }

        bool IsFirstPayload() const { return m_processedFirstPayload; }

        SessionDescription m_currentPalette;

        bool m_processedFirstPayload;
        std::uint64_t m_lastKeepAliveMs;
        int m_keeepAliveIntervalInSeconds;
        int m_sequenceNumber;
        std::string m_url;
        std::string m_session;        
        Surge::SocketHandler m_socketHandler;
        SurgeUtil::StoppableThread m_thread;
    };
    
}

#endif //__RTSP_CLIENT_H__
