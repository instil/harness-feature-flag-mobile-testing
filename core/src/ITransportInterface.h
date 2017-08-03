// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef __NEW_ITRANSPORT_INTERFACE_H__
#define __NEW_ITRANSPORT_INTERFACE_H__

#include "FireableEvent.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"
#include "ISocketHandlerDelegate.h"

#include <string>

namespace Surge {
    
    class ITransportInterface {
    public:
        
        virtual ~ITransportInterface() { }
        
        virtual bool IsInterleavedTransport() = 0;
        
        virtual void RtspTcpOpen(const std::string&, int, std::function<void(int)>) = 0;
        
        virtual void StartRunning() = 0;
        
        virtual void StopRunning() = 0;
        
        virtual bool IsRunning() = 0;
        
        virtual void SetRtpCallback(std::function<void(RtpPacket*)> callback) = 0;
        
        virtual void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) = 0;
        
        virtual void SetDelegate(ISocketHandlerDelegate * const delegate) = 0;
        
        virtual std::string GetTransportHeaderString() const = 0;
    };
    
}

#endif //__NEW_ITRANSPORT_INTERFACE_H__
