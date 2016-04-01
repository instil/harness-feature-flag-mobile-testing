// -*-c++-*-
#ifndef __SOCKET_HANDLER_DELEGATE_H__
#define __SOCKET_HANDLER_DELEGATE_H__

namespace Surge {

    class SocketHandlerDelegate {
    public:

        virtual void SocketReadFailed() = 0;

        virtual void AnnounceReceived() = 0;

        virtual void RedirectReceived() = 0;
        
    };
    
};

#endif //__SOCKET_HANDLER_DELEGATE_H__
