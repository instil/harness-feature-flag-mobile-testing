// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#ifndef Transport_hpp
#define Transport_hpp

#include "ITransportInterface.h"
#include "StoppableThread.h"

#include "uvw.hpp"

namespace Surge {
    class Transport : public ITransportInterface, private SurgeUtil::Runnable {
    
    public:
        Transport(ISocketHandlerDelegate *delegate);
        ~Transport();
        
    public:
        void RtspTcpOpen(const std::string&, int, std::function<void(int)>) override;
        void StartRunning() override;
        void StopRunning() override;
        void RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) override;
        
        void SetDelegate(ISocketHandlerDelegate * const delegate) override {
            m_delegate = delegate;
        }
        
        void SetRtpCallback(std::function<void(RtpPacket*)> callback) override {
            rtpCallback = callback;
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
        
        std::unique_ptr<char[]> generateRtspDataPtr(char *data, size_t length);
        void RtspHandleReceive(const char* buffer, size_t size);
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
        
    protected:
        virtual bool HandlePacket(const char* buffer, size_t size) = 0;
        
    protected:
        std::shared_ptr<uvw::Loop> m_loop;
        std::shared_ptr<uvw::TcpHandle> m_tcp;
        
        SurgeUtil::StoppableThread m_thread;
        std::atomic<bool> m_threadRunning;
        
        std::function<void(Response*)> rtspCallback;
        std::atomic<bool> executingRtspCommand;

        ISocketHandlerDelegate *m_delegate;
        
        std::vector<unsigned char> m_receivedBuffer;
        std::function<void(RtpPacket*)> rtpCallback;
    };
}

#endif /* Transport_hpp */
