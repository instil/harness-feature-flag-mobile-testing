//
//  RtspClient.hpp
//  SurgeCore
//
//  Created by Paul Shields on 15/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef RtspClient_h
#define RtspClient_h

#include <stdio.h>

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

#include "RtspService.h"

namespace Surge {

    class RtspClient : public ISocketHandlerDelegate, public TransportDelegate, private SurgeUtil::Runnable {

    public:
        RtspClient(IRtspClientDelegate * const delegate, bool forceInterleavedTcp = false);

        ~RtspClient();

        void Connect(const std::string& url, std::function<void(bool)> callback);
        void Disconnect();

        void SetCredentials(const std::string& user, const std::string& password);

        void Describe(std::function<void(Surge::DescribeResponse*)> callback);

        void Setup(const SessionDescription& sessionDescription,
                   std::function<void(Surge::SetupResponse*)> callback);

        void Play(std::function<void(Surge::RtspResponse*)> callback = NULL);

        void Pause(std::function<void(Surge::RtspResponse*)> callback = NULL);

        void Options(std::function<void(Surge::RtspResponse*)> callback);

        void Teardown(std::function<void(Surge::RtspResponse*)> callback = NULL);

        void KeepAlive(std::function<void(Surge::RtspResponse*)> callback);

    private:
        void GenerateTransportFromUrl(std::string& url);
        void CreateDepacketizer();

        void StartHousekeepingThread();
        void StopHousekeepingThread();

        void ProcessRtpPacket(const RtpPacket* packet);
        void CheckIfFrameShouldBeDropped(const Surge::RtpPacket* packet);

    private:
        void NotifyDelegateOfExtendedRtpHeader(const unsigned char *extendedHeaderBuffer, size_t length) {
            if (delegate != nullptr) {
                delegate->ClientReceivedExtendedHeader(extendedHeaderBuffer, length);
            }
        }

        void NotifyDelegateOfAvailableFrame(const std::vector<unsigned char> &frame) {
            if (dropFrame) {
                ClearDroppedFrameFlag();
                return;
            }

            if (delegate != nullptr && sendFrameToDelegate()) {
                delegate->ClientReceivedFrame(frame.data(),
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

    private:
        void NotifyDelegateTimeout() {
            GetDispatcher().FailureForClient(this, ERROR_TYPE::LOST_CONNECTION);
        }

        void NotifyDelgateRedirect() {
            GetDispatcher().FailureForClient(this, ERROR_TYPE::REDIRECT);
        }

        void NotifyDelegateAnnounce() {
            GetDispatcher().FailureForClient(this, ERROR_TYPE::ANNOUNCE);
        }

    public:
        IRtspClientDelegate *GetDelegate() const {
            return delegate;
        }

        void SetTimeRange(SurgeUtil::DateTime *startTime, SurgeUtil::DateTime *endTime) {
            SetStartTime(startTime);
            SetEndTime(endTime);
        }

        void SetStartTime(SurgeUtil::DateTime *startTime) {
            if (startTime) {
                if (this->startTime) {
                    delete this->startTime;
                }
                this->startTime = startTime;
            }
        }

        void SetEndTime(SurgeUtil::DateTime *endTime) {
            if (endTime) {
                if (this->endTime) {
                    delete this->endTime;
                }
                this->endTime = endTime;
            }
        }

        void SetSessionDescriptionFactory(SessionDescriptionFactory *factory) {
            rtspService->SetCustomSessionDescriptionFactory(factory);
        }

        int GetTimeout() {
            return this->timeout;
        }

        void SetTimeout(int timeout) {
            this->timeout = timeout;
        }

        bool IsInterleavedTransport() {
            return useInterleavedTcpTransport;
        }

        int GetPacketBufferDelay() {
            return packetBuffer->GetBufferDelay();
        }

        void SetPacketBufferDelay(int bufferDelayMilliseconds) {
            packetBuffer->SetBufferDelay(bufferDelayMilliseconds);
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
            tlsRootCertificateUrl = fileUrl;

            if (tlsClient != nullptr) {
                tlsClient->SetTrustedCertificate(fileUrl);
            }
        }

        void AddAuthenticator(BaseAuthenticator *authenticator) {
            authService->Add(authenticator);
        }

        void RemoveAuthenticator(BaseAuthenticator *authenticator) {
            authService->Remove(authenticator);
        }

        void RemoveAuthenticator(unsigned int index) {
            authService->Remove(index);
        }

        bool RtpPacketTypeIsValid(RtpPacket *packet) {
            // Although RTSP has it's own methods for sending separate streams of data
            // to the client, it's possible (as per the RTP spec) for the RTSP stream to
            // send multiple types of data down the same open port. This simply filters
            // out any unexpected RTP packets (that are likely containing metadata).
            
            return packet->GetType() == sessionDescription.GetPayloadType();
        }

    public: // Inherited from TransportDelegate
        void RtpPacketReceived(RtpPacket *packet) override;

    private: // Inherited from SurgeUtil::Runnable
        void Run() override;

    public: // Inherited from ISocketHandlerDelegate
        void SocketReadFailed() override {
            NotifyDelegateTimeout();
        }

        void AnnounceReceived() override {
            NotifyDelegateAnnounce();
        }

        void RedirectReceived() override {
            NotifyDelgateRedirect();
        }

    private:
        std::string url;
        bool isPlaying;
        bool useInterleavedTcpTransport;

        SurgeUtil::DateTime *startTime;
        SurgeUtil::DateTime *endTime;
        int timeout;
        std::string tlsRootCertificateUrl;

        IRtspClientDelegate *delegate;

        Surge::ITransportInterface *transport;
        Surge::ITLSClient *tlsClient;
        Surge::RtpPacketBuffer *packetBuffer;
        Surge::DispatchQueue *dispatchQueue;
        Surge::Depacketizer *depacketizer;

        bool tlsCertificateValidationEnabled;
        bool tlsSelfSignedCertsAllowed;

        RtspService *rtspService;
        AuthenticationService *authService;

        SessionDescription sessionDescription;
        std::vector<unsigned char> *frameBuffer;

        SurgeUtil::StoppableThread housekeepingThread;
        long long timeLastPacketWasProcessed;
        long long lastKeepAliveMs;
        long long keepAliveIntervalInSeconds;
        bool processedFirstPayload;
        long long lastPacketSequenceNum;
        bool dropFrame;

    private:
        bool shouldSendFrameToDelegate = true;

        bool sendFrameToDelegate() {
            return shouldSendFrameToDelegate;
        }

        void enableVideoFramePublishing() {
            shouldSendFrameToDelegate = true;
        }

        void disableVideoFramePublishing() {
            shouldSendFrameToDelegate = false;
        }
    };

}

#endif /* RtspClient_h */
