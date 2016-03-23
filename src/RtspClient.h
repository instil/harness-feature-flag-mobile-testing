// -*-c++-*-
#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "Surge.h"
#include "ErrorDispatcher.h"
#include "DelegateInterface.h"
#include "StoppableThread.h"
#include "SocketHandler.h"
#include "SessionDescription.h"

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

        SetupResponse* Setup(const SessionDescription sessionDescription, bool serverAllowsAggregate = true);

        RtspResponse* Play(bool waitForResponse = true);

        RtspResponse* Pause();

        RtspResponse* Options();

        RtspResponse* Options(const std::string url);

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
                // casting to char * so swig can see this as a byte[] for jni bindings
                m_delegate->Payload((const char *)buffer, length);
            }
        }

        void NotifyDelegateTimeout() {
            ErrorDispatcher *dispatcher = GetDispatcher();
            if (m_delegate != nullptr) {
                dispatcher->FailureForClient(this, ERROR_TYPE::LOST_CONNECTION);
            }
        }

        void AppendPayloadToCurrentFrame(const unsigned char *buffer, size_t length) {
            size_t old_size = m_currentFrame.size();
            m_currentFrame.resize(old_size + length);
            std::copy(buffer, buffer + length, m_currentFrame.begin() + old_size);
        }

        void ResetCurrentPayload() {
            m_currentFrame.clear();
            m_currentFrame.resize(0);
        }

        int SetupRtspConnection(const std::string url);

        const unsigned char * GetCurrentFrame() const { return &(m_currentFrame[0]); }

        size_t GetCurrentFrameSize() const { return m_currentFrame.size(); }

        void ProcessH264Packet(const RtpPacket* packet);

        void ProcessMP4VPacket(const RtpPacket* packet);

        void ProcessMJPEGPacket(const RtpPacket* packet);

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
