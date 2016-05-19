// -*-c++-*-
#ifndef __RTSP_CLIENT_PROXY_H__
#define __RTSP_CLIENT_PROXY_H__

#include "RtspClient.h"
#include "IRtspClientDelegate.h"
#include "SessionDescription.h"
#include "ILoggingDelegate.h"

// Swig c++ support seems to not understand class instance pointers making the bindings it generates not useful
// this proxy will eliminate the sillyness in the bindings although will need to be maintained.

namespace SurgeJava {

    namespace LoggerProxy {

        void SetLoggingDelegate(const SurgeUtil::ILoggingDelegate *delegate);
        
    };

    namespace RtspClientProxy {
        
        struct RtspClientWrapper {
            Surge::RtspClient *client;
        };

        struct RtspPalette {
            Surge::SessionDescription palette;
        };
        
        struct RtspSessionDescriptions {
            size_t length;
            struct RtspPalette *palettes;
        };

        struct RtspClientWrapper* CreateNewRtspClient(Surge::IRtspClientDelegate *delegate);

        int Describe(struct RtspClientWrapper* const client,
                     const char *url, bool requires_auth, const char *user, const char *pass,
                     struct RtspSessionDescriptions* const palettes);

        int Setup(struct RtspClientWrapper* const client, const RtspPalette *palette);

        int Play(struct RtspClientWrapper* const client);

        int Pause(struct RtspClientWrapper* const client);
        
        void StopClient(struct RtspClientWrapper* client);

        void ReleaseClient(struct RtspClientWrapper* client);

        void ReleaseRtspPaletteDescriptions(struct RtspSessionDescriptions* palettes);
    };
    
};

#endif //__RTSP_CLIENT_PROXY_H__
