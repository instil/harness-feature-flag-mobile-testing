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

#include "DateTime.h"

#include <string>
#include <vector>

namespace Surge {

    class RtspClient : public ISocketHandlerDelegate, private SurgeUtil::Runnable {
    public:
        RtspClient(IRtspClientDelegate * const delegate, bool forceInterleavedTransport = true);

        ~RtspClient();

        void Describe(const std::string& url,
                      std::function<void(Surge::DescribeResponse*)> callback);

        void Describe(const std::string& url,
                      const std::string& user,
                      const std::string& password,
                      std::function<void(Surge::DescribeResponse*)> callback);

        void Setup(const SessionDescription& sessionDescription,
                   bool serverAllowsAggregate,
                   std::function<void(Surge::SetupResponse*)> callback);

        void Play(bool waitForResponse,
                  std::function<void(Surge::RtspResponse*)> callback);

        void Pause(std::function<void(Surge::RtspResponse*)> callback = NULL);

        void Options(std::function<void(RtspResponse*)> callback);

        void Options(const std::string& url,
                     std::function<void(Surge::RtspResponse*)> callback);

        void Teardown(std::function<void(Surge::RtspResponse*)> callback = NULL,
                      bool waitForResponse = false);

        void KeepAlive(std::function<void(Surge::RtspResponse*)> callback);

        void StopClient();
        
        void StopStream();

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

        void SetTimeRange(SurgeUtil::DateTime startDate, SurgeUtil::DateTime endDate) {
            this->startDate = startDate;
            this->startTimeSet = true;
            this->endDate = endDate;
            this->endTimeSet = true;
        }
        
        void SetStartTime(SurgeUtil::DateTime startDate) {
            this->startDate = startDate;
            this->startTimeSet = true;
        }
        
        void SetEndTime(SurgeUtil::DateTime endDate) {
            this->endDate = endDate;
            this->endTimeSet = true;
        }
        
        void ResetTimeToLive() {
            this->startTimeSet = false;
            this->endTimeSet = false;
        }
        
        void SetSessionDescriptionFactory(SessionDescriptionFactory *factory) {
            delete this->factory;
            this->factory = factory;
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
        
        void NotifyDelegateOfExtendedRtpHeader(const unsigned char *extendedHeaderBuffer, size_t length) {
            if (m_delegate != nullptr) {
                m_delegate->ClientReceivedExtendedHeader(extendedHeaderBuffer, length);
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

        void SetupRtspConnection(const std::string& url, std::function<void(int)> callback);

        bool m_isPlaying;
        SurgeUtil::FireableEvent m_abortWait;

        SessionDescription m_sessionDescription;
        std::vector<unsigned char> *frameBuffer;

        Depacketizer *depacketizer;
        IRtspClientDelegate * const m_delegate;
        bool processedFirstPayload;
        long long int m_lastKeepAliveMs;
        int m_keeepAliveIntervalInSeconds;
        int m_sequenceNumber;
        std::string m_url;
        std::string m_session;
        Surge::ITransportInterface *m_transport;
        SurgeUtil::StoppableThread m_thread;
        SurgeUtil::Mutex m_mutex;
        
        bool startTimeSet;
        SurgeUtil::DateTime startDate;
        bool endTimeSet;
        SurgeUtil::DateTime endDate;
        SessionDescriptionFactory *factory;
    };
    
}

#endif //__RTSP_CLIENT_H__
