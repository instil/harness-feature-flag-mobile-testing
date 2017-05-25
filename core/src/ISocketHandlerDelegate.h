// -*-c++-*-
// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __ISOCKET_HANDLER_DELEGATE_H__
#define __ISOCKET_HANDLER_DELEGATE_H__

namespace Surge {

    class ISocketHandlerDelegate {
    public:

        virtual void SocketReadFailed() = 0;

        virtual void AnnounceReceived() = 0;

        virtual void RedirectReceived() = 0;
        
    };
    
};

#endif //__ISOCKET_HANDLER_DELEGATE_H__
