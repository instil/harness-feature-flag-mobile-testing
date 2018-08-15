// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspCommandFactory.h"

#include "DateTime.h"

namespace Surge {

    namespace RtspCommandFactory {

        AuthenticationService *authService;

        void SetAuthService(AuthenticationService *service) {
            authService = service;
        }

        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber) {
            return new class DescribeRequest(url, nextSequenceNumber, authService->AuthHeaders());
        }
        
        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     SurgeUtil::DateTime startDate) {
            return new class DescribeRequest(url, nextSequenceNumber, authService->AuthHeaders(), startDate);
        }

        
        RtspCommand* SetupRequest(const std::string& url,
                                  int nextSequenceNumber,
                                  const ITransportInterface *transport) {
            return new class SetupRequest(url, nextSequenceNumber, transport, authService->AuthHeaders());
        }

        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->AuthHeaders());
        }
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                  SurgeUtil::DateTime startDate) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->AuthHeaders(), startDate);
        }
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 SurgeUtil::DateTime startDate,
                                 SurgeUtil::DateTime endDate) {
            return new class PlayRequest(url, session, nextSequenceNumber, authService->AuthHeaders(), startDate, endDate);
        }

        
        RtspCommand* PauseRequest(const std::string& url,
                                  const std::string& session,
                                  int nextSequenceNumber) {
            return new class PauseRequest(url, session, nextSequenceNumber, authService->AuthHeaders());
        }

        RtspCommand* KeepAliveRequest(const std::string& url,
                                      const std::string& session,
                                      int nextSequenceNumber) {
            return new class OptionsRequest(url, session, nextSequenceNumber, authService->AuthHeaders());
        }

        RtspCommand* OptionsRequest(const std::string& url,
                                    const std::string& session,
                                    int nextSequenceNumber) {
            return new class OptionsRequest(url, session, nextSequenceNumber, authService->AuthHeaders());
        }

        RtspCommand* TeardownRequest(const std::string& url,
                                     const std::string& session,
                                     int nextSequenceNumber) {
            return new class TeardownRequest(url, session, nextSequenceNumber, authService->AuthHeaders());
        }
    }
};
