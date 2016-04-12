// -*-c++-*-
#ifndef __ITRANSPORT_INTERFACE_H__
#define __ITRANSPORT_INTERFACE_H__

#include "FireableEvent.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"

#include <rxcpp/rx.hpp>
#include <string>

namespace Surge {

    class ITransportInterface {
    public:

        virtual ~ITransportInterface() { }

        virtual bool IsInterleavedTransport() = 0;

        virtual bool IsRtpTransportTCP() = 0;
        
        virtual int RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort) = 0;

        virtual void StartRunning() = 0;
        
        virtual void StopRunning() = 0;

        virtual bool IsRunning() = 0;

        virtual void SetTimeout(long timeout) = 0;

        virtual void SetConnectTimeout(long timeout) = 0;

        virtual void SetTransactionTimeout(long timeout) = 0;

        virtual rxcpp::observable<RtpPacket*> GetRtpPacketObservable() const = 0;

        virtual Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true) = 0;

        virtual void SetDelegate(SocketHandlerDelegate * const delegate) = 0;
    
    };
    
}

#endif //__ITRANSPORT_INTERFACE_H__
