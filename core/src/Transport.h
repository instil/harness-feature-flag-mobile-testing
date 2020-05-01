// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef Transport_hpp
#define Transport_hpp

#include "ITransportInterface.h"
#include "StoppableThread.h"
#include "Constants.h"
#include "TransportDelegate.h"

#include "uvw.hpp"

namespace Surge {
    class Transport : public ITransportInterface, private SurgeUtil::Runnable {
    
    public:
        Transport(TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate);
        ~Transport();
        
    public:
        void RtspTcpOpen(SurgeUtil::Url&, std::function<void(int)>) override;
        void StartRunning() override;
        void StopRunning(bool waitUntilStopped = true) override;
        void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) override;
        void ArbitraryDataTransaction(const char *data, const size_t length) override;
        void StartRtspTimer();
        void StopRtspTimer();
        
        void SetDelegate(ISocketHandlerDelegate * const delegate) override {
            m_delegate = delegate;
        }
        
        bool IsRunning() override {
            return m_threadRunning;
        }
        
    private:
        void Run() override;
        
    protected:
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

        void InitializeLibuv();
        void CloseAndCleanUpLibuv();
        void AttachRtspCallbacksToLibuv();
        void StopLibuv();

        void RtspTransaction(const char *data, const size_t length, std::function<void(Response*)> callback);
        std::unique_ptr<char[]> GenerateRtspDataPtr(const char *data, size_t length);
        void SafeRunLibuvCommand(std::function<void()> commandsToRun);
        virtual void RtspHandleReceive(const char* buffer, size_t size);
        bool HandleRtspPacket();
        void AppendDataToBuffer(const char* buffer, size_t size);
        void RemoveDataFromStartOfBuffer(size_t count);
        
        bool BufferContainsRtspPacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "RTSP/1.0", 8) == 0;
        }
        
        bool BufferContainsAnnouncePacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "ANNOUNCE", 8) == 0;
        }
        
        bool BufferContainsRedirectPacket() {
            return m_receivedBuffer.size() >= 8 && strncmp((char*)&(m_receivedBuffer[0]), "REDIRECT", 8) == 0;
        }
        
        std::string ResolveHostnameToIP(const std::string& host, const int port);
        
    protected:
        virtual bool HandlePacket(const char* buffer, size_t size) = 0;
        
    protected:
        std::string m_streamIp;
        uvw::TimerHandle::Time m_rtspTimeoutTime;

        std::shared_ptr<uvw::Loop> m_loop;
        std::shared_ptr<uvw::TcpHandle> m_tcp;
        std::shared_ptr<uvw::TimerHandle> m_timer;
        std::shared_ptr<uvw::AsyncHandle> m_libuvCloser;
        std::shared_ptr<uvw::AsyncHandle> m_libuvCommandNotifier;

        SurgeUtil::StoppableThread m_thread;
        std::atomic<bool> m_threadRunning;
        
        std::function<void(Response*)> rtspCallback;
        std::atomic<bool> executingLibuvCommand;

        ISocketHandlerDelegate *m_delegate;
        TransportDelegate *transportDelegate;
        
        std::vector<unsigned char> m_receivedBuffer;

        SurgeUtil::Mutex threadManipulationMutex;
    };
}

#endif /* Transport_hpp */
