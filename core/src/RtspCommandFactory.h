// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __RTSP_COMMAND_FACTORY_H__
#define __RTSP_COMMAND_FACTORY_H__

#include "Describe.h"
#include "Setup.h"
#include "Play.h"
#include "Pause.h"
#include "Options.h"
#include "Teardown.h"

#include "Base64.h"
#include "ITransportInterface.h"

#include "AuthenticationService.h"

#define Method(e) ({__unused Surge::RtspCommandType _e = e;std::string(#e);})

namespace Surge {

    enum RtspCommandType {
        DESCRIBE,
        SETUP,
        PLAY,
        PAUSE,
        KEEPALIVE,
        OPTIONS,
        TEARDOWN
    };

    namespace RtspCommandFactory {
        
        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     AuthenticationService *authService);

        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     const SurgeUtil::DateTime startDate,
                                     AuthenticationService *authService);
        
        RtspCommand* SetupRequest(const std::string& url,
                                  int nextSequenceNumber,
                                  const ITransportInterface *transport,
                                  AuthenticationService *authService);

        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 AuthenticationService *authService);
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 const SurgeUtil::DateTime startDate,
                                 AuthenticationService *authService);
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 const SurgeUtil::DateTime startDate,
                                 const SurgeUtil::DateTime endDate,
                                 AuthenticationService *authService);

        RtspCommand* PauseRequest(const std::string& url,
                                  const std::string& session,
                                  int nextSequenceNumber,
                                  AuthenticationService *authService);

        RtspCommand* KeepAliveRequest(const std::string& url,
                                      const std::string& session,
                                      int nextSequenceNumber,
                                      AuthenticationService *authService);

        RtspCommand* OptionsRequest(const std::string& url,
                                    const std::string& session,
                                    int nextSequenceNumber,
                                    AuthenticationService *authService);

        RtspCommand* TeardownRequest(const std::string& url,
                                     const std::string& session,
                                     int nextSequenceNumber,
                                     AuthenticationService *authService);
    };
    
}

#endif // __RTSP_COMMAND_FACTORY_H__
