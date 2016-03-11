#include "RtspClientProxy.h"

namespace SurgeJava {

    namespace LoggerProxy {

        void SetLoggingDelegate(const SurgeUtil::LoggingDelegate *delegate) {
            SurgeUtil::Logger& logger = SurgeUtil::Logger::GetInstance();
            logger.SetLoggingDelegate(delegate);
        }
        
    }
    
    namespace RtspClientProxy {

        struct RtspClientWrapper* CreateNewRtspClient(Surge::RtspClientDelegate *delegate) {

            struct RtspClientWrapper* wrapper = (struct RtspClientWrapper*)
                malloc(sizeof(struct RtspClientWrapper));
            
            wrapper->client = new Surge::RtspClient(delegate);

            return wrapper;
        }

        int Describe(struct RtspClientWrapper* const client,
                     const char *url, bool requires_auth, const char *user, const char *pass,
                     struct RtspSessionDescriptions* const palettes) {

            Surge::DescribeResponse* response = client->client->Describe(std::string(url),
                                                                         requires_auth,
                                                                         std::string(user),
                                                                         std::string(pass));
            if (response == nullptr) {
                return -1;
            } else if (!response->Ok()) {
                delete response;
                return -1;
            }

            const std::vector<Surge::SessionDescription> descriptions = response->GetPalettes();
            palettes->length = descriptions.size();
            palettes->palettes = (struct RtspPalette*)malloc(palettes->length * sizeof(struct RtspPalette));

            size_t offs = 0;
            for (auto it = descriptions.begin(); it != descriptions.end(); ++it) {
                Surge::SessionDescription description = *it;
                (palettes->palettes + offs)->palette = description;
                offs++;
            }

            delete response;

            return 0;
        }

        int Setup(struct RtspClientWrapper* const client, const RtspPalette *palette) {
            Surge::SetupResponse* response = client->client->Setup(palette->palette);

            if (response == nullptr) {
                return -1;
            }

            int retval = (response->Ok()) ? 0 : -1;
            delete response;
            
            return retval;
        }

        int Play(struct RtspClientWrapper* const client) {
            Surge::RtspResponse* response = client->client->Play();

            if (response == nullptr) {
                return -1;
            }

            int retval = (response->Ok()) ? 0 : -1;
            delete response;
            
            return retval;
        }

        int Pause(struct RtspClientWrapper* const client) {
            Surge::RtspResponse* response = client->client->Pause();

            if (response == nullptr) {
                return -1;
            }

            int retval = (response->Ok()) ? 0 : -1;
            delete response;
            
            return retval;
        }
        
        void StopClient(struct RtspClientWrapper* client) {
            client->client->StopClient();
        }

        void ReleaseClient(struct RtspClientWrapper* client) {
            delete client->client;
            free(client);
        }

        void ReleaseRtspPaletteDescriptions(struct RtspSessionDescriptions* palettes) {
            free(palettes->palettes);
            free(palettes);
        }
        
    };
};
