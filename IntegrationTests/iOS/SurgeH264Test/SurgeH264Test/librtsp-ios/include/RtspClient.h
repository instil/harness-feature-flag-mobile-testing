// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "DelegateInterface.h"
#include "StoppableThread.h"
#include "SocketHandler.h"
#include "SessionDescription.h"
#include "ErrorDispatcher.h"

#include "DescribeResponse.h"
#include "SetupResponse.h"

#include <string>
#include <vector>

namespace Surge {

    class RtspClient : private SurgeUtil::Runnable {
    public:
        RtspClient(RtspClientDelegate *delegate);

        ~RtspClient();

        DescribeResponse* Describe(const std::string url,
                                   bool requires_auth,
                                   const std::string user,
                                   const std::string password);

        SetupResponse* Setup(const SessionDescription sessionDescription);

        RtspResponse* Play(bool waitForResponse = true);

        RtspResponse* Pause();

        RtspResponse* Options();

        RtspResponse* Teardown(bool waitForResponse = false);

        RtspResponse* KeepAlive();

        void StopClient();

        RtspClientDelegate* GetDelegate() const { return m_delegate; }

    private:
        void Run() override;

        void StartSession();

        void ProcessRtpPacket(const RtpPacket* packet);
        
        int GetNextSequenceNumber() { return m_sequenceNumber++; }

        bool IsFirstPayload() const { return !m_processedFirstPayload; }

        void NotifyDelegatePayload(const unsigned char *buffer, size_t length) {
            if (m_delegate != nullptr) {
                m_delegate->Payload(buffer, length);
            }
        }

        void NotifyDelegateTimeout() {
            if (m_delegate != nullptr) {
                // TODO
            }
        }

        void AppendPayloadToCurrentFrame(const unsigned char *buffer, size_t length) {
            size_t i;
            for (i = 0; i < length; ++i) {
                m_currentFrame.push_back(buffer[i]);
            }
        }

        void ResetCurrentPayload() {
            m_currentFrame.clear();
        }

        const unsigned char * GetCurrentFrame() const { return &(m_currentFrame[0]); }

        size_t GetCurrentFrameSize() const { return m_currentFrame.size(); }

        std::vector<unsigned char> m_currentFrame;
        SessionDescription m_currentPalette;

        RtspClientDelegate *m_delegate;
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