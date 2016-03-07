// -*-c++-*-
#ifndef __DELEGATE_INTERFACE_H__
#define __DELEGATE_INTERFACE_H__

#include <cstdlib>

namespace Surge {

    class RtspClientDelegate {
    public:

        virtual ~RtspClientDelegate() { };
        
        virtual void ClientDidTimeout() = 0;

        virtual void Payload(const unsigned char* buffer, size_t length) = 0;

    };
    
};

#endif //__DELEGATE_INTERFACE_H__
