// -*-c++-*-
#ifndef __SOCKET_HANDLER_H__
#define __SOCKET_HANDLER_H__

#include "Transport.h"
#include "ITransportInterface.h"
#include "StoppableThread.h"
#include "RtspCommand.h"
#include "Response.h"
#include "RtpPacket.h"

#include "Mutex.h"
#include "DataEventQueue.h"

#include <string>
#include <rxcpp/rx.hpp>

namespace Surge {
    
    class SocketHandler : public Transport, public ITransportInterface, private SurgeUtil::Runnable {
    public:
        SocketHandler(SocketHandlerDelegate * delegate);

        ~SocketHandler();

        void StartRunning() override;
        
        void StopRunning() override;

        bool IsRunning() override { return m_thread.IsRunning(); }

        int RtspTcpOpen(const std::string& host, int port, const SurgeUtil::FireableEvent& abort) override;

        Response* RtspTransaction(const RtspCommand* command, bool waitForResponse = true);

        rxcpp::observable<RtpPacket*> GetRtpPacketObservable() const override { return m_rtpPacketSubject.get_observable(); }

        void SetRtpInterleavedChannel(int channel) { m_rtpInterleavedChannel = channel; }

        void SetRtcpInterleavedChannel(int channel) { m_rtcpInterleavedChannel = channel; }

        void SetTimeout(long timeout) override { m_timeoutMs = timeout; }
        
        void SetConnectTimeout(long timeout) override { m_connectTimeoutMs = timeout; }

        void SetTransactionTimeout(long timeout) override { m_connectTimeoutMs = timeout; }

    private:
        bool ProcessSend(const int fd, const unsigned char *bytes, size_t length);

        void HandleReceive(const SurgeUtil::WaitableEvent& event);

        void WaitForSendEventToBeHandled();
        
        void Run() override;        
        
        int m_rtpInterleavedChannel;
        int m_rtcpInterleavedChannel;

        std::vector<unsigned char> m_receivedBuffer;
        
        SurgeUtil::Mutex m_mutex;
        
        SurgeUtil::DataEventQueue<Response*> m_rtspOutputQueue;
        rxcpp::subjects::subject<RtpPacket*> m_rtpPacketSubject;

        std::atomic<bool> m_running;
        
        int m_rtspSocketFD;
        size_t m_readBufferSize;
        long m_connectTimeoutMs;
        long m_transactionTimeoutMs;
        long m_timeoutMs;

        SurgeUtil::StoppableThread m_thread;
    };
}

#endif //__SOCKET_HANDLER_H__
