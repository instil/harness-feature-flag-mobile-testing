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

        void SetBasicAuthCredentials(const char *user, const char *password);

        bool IsBasicAuthHeaderAvailable();
        
        const std::string& GetBasicAuthHeader();

        void ResetSession();
        
        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber);

        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     const SurgeUtil::DateTime startDate);
        
        RtspCommand* SetupRequest(const std::string& url,
                                  int nextSequenceNumber,
                                  const ITransportInterface *transport);

        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber);
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 const SurgeUtil::DateTime startDate);
        
        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber,
                                 const SurgeUtil::DateTime startDate,
                                 const SurgeUtil::DateTime endDate);

        RtspCommand* PauseRequest(const std::string& url,
                                  const std::string& session,
                                  int nextSequenceNumber);

        RtspCommand* KeepAliveRequest(const std::string& url,
                                      const std::string& session,
                                      int nextSequenceNumber);

        RtspCommand* OptionsRequest(const std::string& url,
                                    const std::string& session,
                                    int nextSequenceNumber);

        RtspCommand* TeardownRequest(const std::string& url,
                                     const std::string& session,
                                     int nextSequenceNumber);
    };
    
}

#endif // __RTSP_COMMAND_FACTORY_H__
