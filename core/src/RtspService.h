//
//  RtspService.h
//  SurgeCore
//
//  Created by Paul Shields on 16/08/2018.
//  Copyright © 2018 Instil. All rights reserved.
//

#ifndef RtspService_h
#define RtspService_h

#include <stdio.h>

#include "ITransportInterface.h"
#include "AuthenticationService.h"
#include "DescribeResponse.h"
#include "DateTime.h"
#include "SessionDescriptionFactory.h"
#include "RtspCommandFactory.h"
#include "SessionDescription.h"

namespace Surge {
    class RtspService {
    public:
        RtspService();
        ~RtspService();

        void Describe(const SurgeUtil::DateTime *startTime, std::function<void(Surge::DescribeResponse*)> callback);

        void Setup(const SessionDescription& sessionDescription, std::function<void(Surge::SetupResponse*)> callback);

        void Play(const SurgeUtil::DateTime *startTime, const SurgeUtil::DateTime *endTime, std::function<void(Surge::RtspResponse*)> callback);

        void Pause(std::function<void(Surge::RtspResponse*)> callback);

        void Options(std::function<void(Surge::RtspResponse*)> callback);

        void Teardown(std::function<void(Surge::RtspResponse*)> callback);

        void KeepAlive(std::function<void(Surge::RtspResponse*)> callback);

    public:
        void SetStreamTransport(ITransportInterface *transport, std::string streamUrl) {
            this->streamUrl = streamUrl;
            this->transport = transport;

            sequenceNumber = 1;
        }

        void SetAuthService(AuthenticationService *authService) {
            this->authService = authService;
        }

        void SetCustomSessionDescriptionFactory(SessionDescriptionFactory *factory) {
            delete this->sessionDescriptionFactory;
            this->sessionDescriptionFactory = factory;
        }

    private:
        unsigned int NextSequenceNumber() {
            return sequenceNumber++;
        }

    private:
        std::string streamUrl;

        ITransportInterface *transport;
        AuthenticationService *authService;
        SessionDescriptionFactory *sessionDescriptionFactory;

        unsigned int sequenceNumber;
        std::string session;
    };
}


#endif /* RtspService_h */
