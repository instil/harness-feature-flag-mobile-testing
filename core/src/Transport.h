// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

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

        bool executingRtspCommand = true;
    };
}

#endif
