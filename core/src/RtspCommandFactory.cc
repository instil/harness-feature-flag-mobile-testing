// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspCommandFactory.h"

#include "DateTime.h"

namespace Surge {

    namespace RtspCommandFactory {

        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     AuthenticationService *authService) {
            return new class DescribeRequest(url, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(DESCRIBE)));
        }
        
        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     SurgeUtil::DateTime startDate,
                                     AuthenticationService *authService) {
            return new class DescribeRequest(url, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(DESCRIBE)), startDate);
        }

        
        RtspCommand* SetupRequest(const std::string& url,
                                  int nextSequenceNumber,
                                  const ITransportInterface *transport,
                                  AuthenticationService *authService) {
            return new class SetupRequest(url, nextSequenceNumber, transport, authService->GenerateAuthHeadersFor(Method(SETUP)));
        }

        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 AuthenticationService *authService) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(PLAY)));
        }
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                  SurgeUtil::DateTime startDate,
                                AuthenticationService *authService) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(PLAY)), startDate);
        }
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 SurgeUtil::DateTime startDate,
                                 SurgeUtil::DateTime endDate,
                                AuthenticationService *authService) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(PLAY)), startDate, endDate);
        }

        
        RtspCommand* PauseRequest(const std::string& url,
                                  const std::string& session,
                                  int nextSequenceNumber,
                                  AuthenticationService *authService) {
            return new class PauseRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(PAUSE)));
        }

        RtspCommand* KeepAliveRequest(const std::string& url,
                                      const std::string& session,
                                      int nextSequenceNumber,
                                      AuthenticationService *authService) {
            return new class OptionsRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(OPTIONS)));
        }

        RtspCommand* OptionsRequest(const std::string& url,
                                    const std::string& session,
                                    int nextSequenceNumber,
                                    AuthenticationService *authService) {
            return new class OptionsRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(OPTIONS)));
        }

        RtspCommand* TeardownRequest(const std::string& url,
                                     const std::string& session,
                                     int nextSequenceNumber,
                                     AuthenticationService *authService) {
            return new class TeardownRequest(url, session, nextSequenceNumber, authService->GenerateAuthHeadersFor(Method(TEARDOWN)));
        }
    }
};
