// -*-c++-*-
#ifndef __RTSP_COMMAND_FACTORY_H__
#define __RTSP_COMMAND_FACTORY_H__

#include "Describe.h"

#include "Base64.h"

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
        
        static std::string __rtspCommandFactoryBasicAuthString;
        
        void SetBasicAuthCredentials(const char *user,
                                            const char *password)
        {
            __rtspCommandFactoryBasicAuthString = "Authorization: Basic " +
                SurgeUtil::Base64Encode(
                    std::string(user) + ":" + std::string(password)
                )
                + "\r\n";
        }

        bool IsBasicAuthHeaderAvailable() {
            return !__rtspCommandFactoryBasicAuthString.empty();
        }
        
        const std::string GetBasicAuthHeader() {
            return __rtspCommandFactoryBasicAuthString;
        }

        void ResetSession() {
            __rtspCommandFactoryBasicAuthString.clear();
        }
        
        RtspCommand* DescribeRequest(const std::string url,
                                           int nextSequenceNumber,
                                           bool isLive) {
            return new class DescribeRequest(url, nextSequenceNumber, isLive, GetBasicAuthHeader());
        }

        /*static RtspCommand& SetupRequest() {
            
        }

        static RtspCommand& PlayRequest() {
            
        }

        static RtspCommand& PauseRequest() {
            
        }

        static RtspCommand& KeepAliveRequest() {
            
        }

        static RtspCommand& OptionsRequest() {
            
        }

        static RtspCommand& TeardownRequest() {
            
        }*/
    };
    
}

#endif // __RTSP_COMMAND_FACTORY_H__
