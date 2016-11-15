//
//  UvwTransport.cpp
//  SurgeCore
//
//  Created by Paul Shields on 14/11/2016.
//  Copyright © 2016 Instil. All rights reserved.
//

#include "UvwTransport.h"

Surge::UvwTransport::UvwTransport(ISocketHandlerDelegate *delegate) : m_running(false),
                                                        m_delegate(delegate),
                                                        m_connectTimeoutMs(DEFAULT_CONNECT_TIMEOUT_MS),
                                                        m_transactionTimeoutMs(DEFAULT_TRANSACTION_TIMEOUT_MS),
                                                        m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS) {
    m_loop = uvw::Loop::getDefault();
    
    m_tcp = uvw::TcpHandle::create(m_loop);
    m_tcp->init();
}

Surge::UvwTransport::~UvwTransport() {
    StopRunning();
    
    m_rtspOutputQueue.Flush([&] (Response* resp) {
        delete resp;
    });
    
    m_tcp->close();
    m_loop->close();
}


void Surge::UvwTransport::StartRunning() {
    if (IsRunning()) {
        return;
    }
    m_running = true;
    m_thread.Execute(*this);
}

void Surge::UvwTransport::StopRunning() {
    if (!IsRunning()) {
        return;
    }
    m_running = false;
    m_thread.Stop();
}


void Surge::UvwTransport::RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) {
    
    m_tcp->once<uvw::ErrorEvent>([&](const uvw::ErrorEvent &error, uvw::TcpHandle &tcp) {
        ERROR("Error occured on connect");
        callback(error.code());
    });
    
    m_tcp->once<uvw::ConnectEvent>([&](const uvw::ConnectEvent &connectEvent, uvw::TcpHandle &tcp) mutable {
        INFO("Connected");
        callback(0);
    });
    
    INFO("Connecting to TCP port");
    m_tcp->connect(host, port);
    m_loop->run<uvw::Loop::Mode::ONCE>();
}

Surge::Response* Surge::UvwTransport::RtspTransaction(const RtspCommand* command, bool waitForResponse) {
    DEBUG("Sending command to server");
  
    m_tcp->write(generateRtspDataPtr((char *)command->BytesPointer(), command->PointerLength()),
                 command->PointerLength());
    
    // To be replaced by a callback instead?
    
    Surge::Response *response;
    
    auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({&m_rtspOutputQueue.GetNonEmptyEvent()},
                                                          m_transactionTimeoutMs);
    if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_rtspOutputQueue.GetNonEmptyEvent())) {
        response = m_rtspOutputQueue.RemoveItem();
        
        DEBUG("TRANSACTION RESPONSE: " << response->StringDump());
    }
    return response;
}

std::unique_ptr<char[]> Surge::UvwTransport::generateRtspDataPtr(char *data, size_t length) {
    char *dataCopy = new char[length];
    memcpy(dataCopy, data, length);
    return std::unique_ptr<char[]>(dataCopy);
}


void Surge::UvwTransport::Run() {
    m_tcp->on<uvw::ErrorEvent>([](const uvw::ErrorEvent &error, uvw::TcpHandle &tcp) {
        ERROR("ERROR");
    });
    
    m_tcp->on<uvw::ListenEvent>([](const uvw::ListenEvent &listenEvent, uvw::TcpHandle &tcp) mutable {
        INFO("Listening");
    });
    
    m_tcp->on<uvw::ShutdownEvent>([](const uvw::ShutdownEvent &shutdownEvent, uvw::TcpHandle &tcp) mutable {
        INFO("Shutdown");
    });
    
    m_tcp->on<uvw::WriteEvent>([](const uvw::WriteEvent &writeEvent, uvw::TcpHandle &tcp) mutable {
        DEBUG("Sent request to stream, wait for a response");
        tcp.read();
    });
    
    m_tcp->on<uvw::DataEvent>([&](const uvw::DataEvent &dataEvent, uvw::TcpHandle &tcp) mutable {
        DEBUG("Data received, handing received data");
        RtspHandleReceive(dataEvent.data.get(), dataEvent.length);
    });
    
    m_tcp->on<uvw::EndEvent>([](const uvw::EndEvent &endEvent, uvw::TcpHandle &tcp) mutable {
        INFO("Loop ended, closing TCP port");
        tcp.close();
    });
    
    while (m_running) {
        INFO("RUNNING");
        m_loop->run();
    }
    
    m_tcp->close();
}
