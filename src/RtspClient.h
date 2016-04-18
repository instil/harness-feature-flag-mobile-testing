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

#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "Surge.h"
#include "ErrorDispatcher.h"
#include "IRtspClientDelegate.h"
#include "StoppableThread.h"
#include "Transport.h"
#include "ITransportInterface.h"
#include "SessionDescription.h"
#include "ISocketHandlerDelegate.h"

#include "DescribeResponse.h"
#include "SetupResponse.h"

#include <string>
#include <vector>


namespace Surge {

    class RtspClient : public ISocketHandlerDelegate, private SurgeUtil::Runnable {
    public:
        RtspClient(IRtspClientDelegate * const delegate);

        RtspClient(IRtspClientDelegate * const delegate, Surge::ITransportInterface * const transport);

        ~RtspClient();

        DescribeResponse* Describe(const std::string& url,
                                   bool requires_auth,
                                   const std::string& user,
                                   const std::string& password);

        SetupResponse* Setup(const SessionDescription& sessionDescription, bool serverAllowsAggregate = true);

        RtspResponse* Play(bool waitForResponse = true);

        RtspResponse* Pause();

        RtspResponse* Options();

        RtspResponse* Options(const std::string& url);

        RtspResponse* Teardown(bool waitForResponse = false);

        RtspResponse* KeepAlive();

        void StopClient();

        IRtspClientDelegate* GetDelegate() const { return m_delegate; }

        void SocketReadFailed() override {
            NotifyDelegateTimeout();
        }

        void AnnounceReceived() override {
            NotifyDelegateAnnounce();
        }

        void RedirectReceived() override {
            NotifyDelgateRedirect();
        }

        void SetNoPacketTimeout(long timeout) {
            m_noPacketTimeout = timeout;
        }

        void SetTransactionTimeout(long timeout) {
            m_transport->SetTransactionTimeout(timeout);
        }

        void SetConnectTimeout(long timeout) {
            m_transport->SetConnectTimeout(timeout);
        }

    private:
        void Run() override;

        void StartSession();

        void Abort() {
            m_abortWait.Fire();
        }

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
            GetDispatcher().FailureForClient(this, ERROR_TYPE::LOST_CONNECTION);
        }

        void NotifyDelgateRedirect() {
            GetDispatcher().FailureForClient(this, ERROR_TYPE::REDIRECT);
        }

        void NotifyDelegateAnnounce() {
            GetDispatcher().FailureForClient(this, ERROR_TYPE::ANNOUNCE);
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

        int SetupRtspConnection(const std::string& url);

        const unsigned char * GetCurrentFrame() const { return &(m_currentFrame[0]); }

        size_t GetCurrentFrameSize() const { return m_currentFrame.size(); }

        void ProcessH264Packet(const RtpPacket* packet);

        void ProcessMP4VPacket(const RtpPacket* packet);

        void ProcessMJPEGPacket(const RtpPacket* packet);

        bool m_isPlaying;
        SurgeUtil::FireableEvent m_abortWait;

        std::vector<unsigned char> m_currentFrame;
        SessionDescription m_currentPalette;

        IRtspClientDelegate * const m_delegate;
        long m_noPacketTimeout;
        bool m_processedFirstPayload;
        std::uint64_t m_lastKeepAliveMs;
        int m_keeepAliveIntervalInSeconds;
        int m_sequenceNumber;
        std::string m_url;
        std::string m_session;
        Surge::ITransportInterface *m_transport;
        SurgeUtil::StoppableThread m_thread;
        SurgeUtil::Mutex m_mutex;
    };
    
}

#endif //__RTSP_CLIENT_H__
