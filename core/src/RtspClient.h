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
#include "Depacketizer.h"

#include <string>
#include <vector>


namespace Surge {

    class RtspClient : public ISocketHandlerDelegate, private SurgeUtil::Runnable {
    public:
        RtspClient(IRtspClientDelegate * const delegate);

        RtspClient(IRtspClientDelegate * const delegate, Surge::ITransportInterface * const transport);

        ~RtspClient();

        DescribeResponse* Describe(const std::string& url);

        DescribeResponse* Describe(const std::string& url,
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

        void CreateDepacketizer();
        
        int GetNextSequenceNumber() { return m_sequenceNumber++; }

        void NotifyDelegateOfAvailableFrame() {
            if (m_delegate != nullptr) {
                m_delegate->ClientReceivedFrame(frameBuffer->data(),
                                                frameBuffer->size(),
                                                depacketizer->GetWidth(),
                                                depacketizer->GetHeight(),
                                                1,
                                                1);
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

        void ClearFrameBuffer() {
            frameBuffer->clear();
            frameBuffer->resize(0);
        }

        int SetupRtspConnection(const std::string& url);

        bool m_isPlaying;
        SurgeUtil::FireableEvent m_abortWait;

        SessionDescription m_sessionDescription;
        std::vector<unsigned char> *frameBuffer;

        Depacketizer *depacketizer;
        IRtspClientDelegate * const m_delegate;
        long m_noPacketTimeout;
        bool processedFirstPayload;
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
