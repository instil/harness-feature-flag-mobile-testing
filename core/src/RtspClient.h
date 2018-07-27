// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "Surge.h"
#include "ErrorDispatcher.h"
#include "IRtspClientDelegate.h"
#include "StoppableThread.h"
#include "ITransportInterface.h"
#include "SessionDescription.h"
#include "ISocketHandlerDelegate.h"
#include "TransportDelegate.h"

#include "DescribeResponse.h"
#include "SetupResponse.h"
#include "Depacketizer.h"
#include "RtpPacketBuffer.h"
#include "DispatchQueue.h"

#include "ITLSClient.h"

#include "AuthenticationService.h"

#include "DateTime.h"

#include <string>
#include <vector>

namespace Surge {

    class RtspClient : public ISocketHandlerDelegate, public TransportDelegate, private SurgeUtil::Runnable {
    public:
        RtspClient(IRtspClientDelegate * const delegate, bool useInterleavedTcpTransport = false);

        ~RtspClient();

        void SetTransport(const std::string& url);

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

        void ClearStartTime() {
            this->startTimeSet = false;
        }

        void ClearEndTime() {
            this->endTimeSet = false;
        }
        
        void ResetTimeToLive() {
            this->startTimeSet = false;
            this->endTimeSet = false;
        }
        
        void SetSessionDescriptionFactory(SessionDescriptionFactory *factory) {
            delete this->factory;
            this->factory = factory;
        }

        int GetTimeout() {
            return this->m_timeout;
        }

        void SetTimeout(int timeout) {
            this->m_timeout = timeout;
        }
        
        bool IsInterleavedTransport() {
            return useInterleavedTcpTransport;
        }

        void SetTLSCertificateValidationEnabled(bool tlsCertificateValidationEnabled) {
            this->tlsCertificateValidationEnabled = tlsCertificateValidationEnabled;
        }

        bool IsTLSCertificateValidationEnabled() {
            return tlsCertificateValidationEnabled;
        }

        void SetTLSSelfSignedCertsAllowed(bool tlsSelfSignedCertsAllowed) {
            this->tlsSelfSignedCertsAllowed = tlsSelfSignedCertsAllowed;
        }

        bool TLSSelfSignedCertsAllowed() {
            return tlsSelfSignedCertsAllowed;
        }

        void SetTLSTrustedCertificate(const std::string& fileUrl) {
            if (tlsClient != nullptr) {
                tlsClient->SetTrustedCertificate(fileUrl);
            }
        }

    private:
        void Run() override;

        void StartSession();

        void ProcessRtpPacket(const RtpPacket* packet);

        void CreateDepacketizer();
        
        int GetNextSequenceNumber() { return m_sequenceNumber++; }

        void NotifyDelegateOfAvailableFrame(const std::vector<unsigned char> &frame) {
            if (dropFrame) {
                ClearDroppedFrameFlag();
                return;
            }
            
            if (m_delegate != nullptr) {
                m_delegate->ClientReceivedFrame(frame.data(),
                                                frame.size(),
                                                depacketizer->GetWidth(),
                                                depacketizer->GetHeight(),
                                                1,
                                                1);
            }
        }
        
        void DropNextFrame() {
            dropFrame = true;
        }
        
        void ClearDroppedFrameFlag() {
            dropFrame = false;
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

        void SetupRtspConnection(const std::string& url, std::function<void(int)> callback);

        bool m_isPlaying;

        bool useInterleavedTcpTransport;

        SessionDescription m_sessionDescription;
        std::vector<unsigned char> *frameBuffer;

        Depacketizer *depacketizer;
        IRtspClientDelegate * const m_delegate;
        bool processedFirstPayload;
        long long int m_lastKeepAliveMs;
        int m_keeepAliveIntervalInSeconds;
        int m_sequenceNumber;
        int m_timeout;
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

        Surge::DispatchQueue *dispatchQueue;

        Surge::ITLSClient *tlsClient;
        bool tlsCertificateValidationEnabled;
        bool tlsSelfSignedCertsAllowed;

        AuthenticationService *authService;

        void RtpPacketReceived(RtpPacket *packet) override;
                
    public:
        Surge::RtpPacketBuffer *packetBuffer;
        
        int GetPacketBufferDelay() {
            return packetBuffer->GetBufferDelay();
        }
        
        void SetPacketBufferDelay(int bufferDelayMilliseconds) {
            packetBuffer->SetBufferDelay(bufferDelayMilliseconds);
        }
        
    public:
        void CheckIfFrameShouldBeDropped(const Surge::RtpPacket* packet);
        long long lastPacketSequenceNum = MAX_SEQ_NUM;
        bool dropFrame = false;
    };
    
}

#endif //__RTSP_CLIENT_H__
