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

#ifndef __UVW_TRANSPORT_H__
#define __UVW_TRANSPORT_H__

#include "ITransportInterface.h"
#include "Constants.h"
//#include "Thread.h"
#include "StoppableThread.h"
#include "DataEventQueue.h"

#include "uvw.hpp"

#include <stdio.h>

using SurgeUtil::Constants::DEFAULT_SOCKET_HANDLER_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_CONNECT_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_TRANSACTION_TIMEOUT_MS;

namespace Surge {
    
    class Transport : public ITransportInterface, private SurgeUtil::Runnable {
        
    public:
        
        Transport(ISocketHandlerDelegate *delegate);
        ~Transport();

        void RtspTcpOpen(const std::string&, int, std::function<void(int)>) override;
        
        void StartRunning () override;
        void StopRunning() override;
        
        void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) override;
        
        
        virtual bool IsInterleavedTransport() override = 0;
        virtual bool IsRtpTransportTCP() override = 0;
        virtual std::string GetTransportHeaderString() const override = 0;
        virtual void RtspHandleReceive(const char* buffer, size_t size) = 0;
        
        
        bool IsRunning() override { return m_thread.IsRunning(); }
        
        void SetRtpCallback(std::function<void(RtpPacket*)> callback) override {
            rtpCallback = callback;
        }
        
        void SetDelegate(ISocketHandlerDelegate * const delegate) override {
            m_delegate = delegate;
        }
        

        void NotifyDelegateOfReadFailure() {
            if (m_delegate != nullptr) {
                m_delegate->SocketReadFailed();
            }
        }
        
        void NotifyDelegateOfAnnounce() {
            if (m_delegate != nullptr) {
                m_delegate->AnnounceReceived();
            }
        }
        
        void NotifyDelegateOfRedirect() {
            if (m_delegate != nullptr) {
                m_delegate->RedirectReceived();
            }
        }
        
    protected:
        void Run() override;
        
    protected:
        std::shared_ptr<uvw::Loop> m_loop;
        std::shared_ptr<uvw::TcpHandle> m_tcp;
        
        SurgeUtil::StoppableThread m_thread;
        std::atomic<bool> m_running;
        
        std::function<void(RtpPacket*)> rtpCallback;
        std::function<void(Response*)> rtspCallback;
        ISocketHandlerDelegate *m_delegate;
   
    private:
        std::unique_ptr<char[]> generateRtspDataPtr(char *, size_t);
    };
}

#endif
