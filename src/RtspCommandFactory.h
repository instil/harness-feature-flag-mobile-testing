// -*-c++-*-
#ifndef __RTSP_COMMAND_FACTORY_H__
#define __RTSP_COMMAND_FACTORY_H__

#include "RtspCommand.h"
#include "Describe.h"

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

    class RtspCommandFactory {
    public:

        static void SetBasicAuthCredentials(const char *user,
                                            const char *password) {
            m_basicAuthString = "Authorization: Basic " +
                SurgeUtil::Base64Encode(
                    std::string(user) + ":" + std::string(password)
                )
                + "\r\n";
        }

        static bool IsBasicAuthHeaderAvailable() {
            return !m_basicAuthString.empty();
        }
        
        static const std::string GetBasicAuthHeader() {
            return m_basicAuthString;
        }

        static void ResetSession() {
            m_basicAuthString.clear();
        }

        static RtspCommand& DescribeRequest(const std::string url,
                                            int nextSequenceNumber,
                                            bool isLive) {
            return DescribeRequest(url, nextSequenceNumber, isLive);
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

    private:
        static std::string m_basicAuthString;
    };
    
}

#endif // __RTSP_COMMAND_FACTORY_H__
