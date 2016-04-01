// -*-c++-*-
#ifndef __DELEGATE_INTERFACE_H__
#define __DELEGATE_INTERFACE_H__

#include <cstdlib>

namespace Surge {

    class RtspClientDelegate {
    public:

        virtual ~RtspClientDelegate() { };

        // TIMEOUT
        virtual void ClientDidTimeout() = 0;

        // ANNOUNCE / REDIRECT
        virtual void StreamConfigChanged(bool wasRedirect) = 0;

        // VIDEO
        virtual void Payload(const char* frameBuffer, size_t length) = 0;

    };
    
};

#endif //__DELEGATE_INTERFACE_H__
