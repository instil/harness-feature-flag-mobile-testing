#include "RtspCommandFactory.h"

namespace Surge {

    namespace RtspCommandFactory {

        static std::string __rtspCommandFactoryBasicAuthString;

        void SetBasicAuthCredentials(const char *user, const char *password)
        {
            std::string auth = std::string(user) + ":" + std::string(password);
            __rtspCommandFactoryBasicAuthString = "Authorization: Basic " +
                SurgeUtil::Base64Encode((const unsigned char *)auth.c_str(), auth.length())
                + "\r\n";
        }

        bool IsBasicAuthHeaderAvailable() {
            return !__rtspCommandFactoryBasicAuthString.empty();
        }
        
        const std::string& GetBasicAuthHeader() {
            return __rtspCommandFactoryBasicAuthString;
        }

        void ResetSession() {
            __rtspCommandFactoryBasicAuthString.clear();
        }
        
        RtspCommand* DescribeRequest(const std::string& url,
                                     int nextSequenceNumber,
                                     bool isLive) {
            return new class DescribeRequest(url, nextSequenceNumber, isLive, GetBasicAuthHeader());
        }

        RtspCommand* SetupRequest(const std::string& url,
                                  int nextSequenceNumber,
                                  const ITransportInterface *transport) {
            return new class SetupRequest(url, nextSequenceNumber, transport, GetBasicAuthHeader());
        }

        RtspCommand* PlayRequest(const std::string& url,
                                 const std::string& session,
                                 int nextSequenceNumber) {
            return new class PlayRequest(url, session, nextSequenceNumber, GetBasicAuthHeader());
        }

        RtspCommand* PauseRequest(const std::string& url,
                                  const std::string& session,
                                  int nextSequenceNumber) {
            return new class PauseRequest(url, session, nextSequenceNumber, GetBasicAuthHeader());
        }

        RtspCommand* KeepAliveRequest(const std::string& url,
                                      const std::string& session,
                                      int nextSequenceNumber) {
            return new class OptionsRequest(url, session, nextSequenceNumber, GetBasicAuthHeader());
        }

        RtspCommand* OptionsRequest(const std::string& url,
                                    const std::string& session,
                                    int nextSequenceNumber) {
            return new class OptionsRequest(url, session, nextSequenceNumber, GetBasicAuthHeader());
        }

        RtspCommand* TeardownRequest(const std::string& url,
                                     const std::string& session,
                                     int nextSequenceNumber) {
            return new class TeardownRequest(url, session, nextSequenceNumber, GetBasicAuthHeader());
        }
    }
};
