// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __ITRANSPORT_INTERFACE_H__
#define __ITRANSPORT_INTERFACE_H__

#include "FireableEvent.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"
#include "SocketHandlerDelegate.h"

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

        virtual std::string GetTransportHeaderString() const = 0;
    };
    
}

#endif //__ITRANSPORT_INTERFACE_H__
