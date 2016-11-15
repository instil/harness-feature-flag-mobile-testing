//
//  UvwTransport.hpp
//  SurgeCore
//
//  Created by Paul Shields on 14/11/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

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
    
    class UvwTransport : public ITransportInterface, private SurgeUtil::Runnable {
        
    public:
        
        UvwTransport(ISocketHandlerDelegate *delegate);
        ~UvwTransport();

        void RtspTcpOpen(const std::string&, int, std::function<void(int)>) override;
        
        void StartRunning () override;
        void StopRunning() override;

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true) override;

    public:
        
        bool IsRunning() override { return m_thread.IsRunning(); }
        
        void SetTimeout(long timeout) override { m_timeoutMs = timeout; }
        
        void SetConnectTimeout(long timeout) override { m_connectTimeoutMs = timeout; }
        
        void SetTransactionTimeout(long timeout) override { m_connectTimeoutMs = timeout; }
        
        rxcpp::observable<RtpPacket*> GetRtpPacketObservable() const override {
            return m_rtpPacketSubject.get_observable();
        }
        
        void SetDelegate(ISocketHandlerDelegate * const delegate) override {
            m_delegate = delegate;
        }
        
    public:
        virtual bool IsInterleavedTransport() override = 0;
        virtual bool IsRtpTransportTCP() override = 0;
        virtual std::string GetTransportHeaderString() const override = 0;
        
    public:
        virtual void RtspHandleReceive(const char* buffer, size_t size) = 0;
        
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
        
        long m_connectTimeoutMs;
        long m_transactionTimeoutMs;
        long m_timeoutMs;
        
        rxcpp::subjects::subject<RtpPacket*> m_rtpPacketSubject;
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
        ISocketHandlerDelegate *m_delegate;
   
    private:
        std::unique_ptr<char[]> generateRtspDataPtr(char *, size_t);
    };
}

#endif
