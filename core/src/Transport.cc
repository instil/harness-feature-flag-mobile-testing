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

#include "Transport.h"

Surge::Transport::Transport(ISocketHandlerDelegate *delegate) : m_running(false),
                                                        m_delegate(delegate),
                                                        m_connectTimeoutMs(DEFAULT_CONNECT_TIMEOUT_MS),
                                                        m_transactionTimeoutMs(DEFAULT_TRANSACTION_TIMEOUT_MS),
                                                        m_timeoutMs(DEFAULT_SOCKET_HANDLER_TIMEOUT_MS)
{
    m_loop = uvw::Loop::getDefault();
    
    m_tcp = uvw::TcpHandle::create(m_loop);
    m_tcp->init();
    
    std::shared_ptr<uvw::UDPHandle> test = m_loop->resource<uvw::UDPHandle>();
}

Surge::Transport::~Transport() {
    StopRunning();
    
    m_rtspOutputQueue.Flush([&] (Response* resp) {
        delete resp;
    });
    
    m_tcp->close();
    m_loop->close();
}


void Surge::Transport::StartRunning() {
    if (IsRunning()) {
        return;
    }
    m_running = true;
    m_thread.Execute(*this);
}

void Surge::Transport::StopRunning() {
    if (!IsRunning()) {
        return;
    }
    m_running = false;
    m_thread.Stop();
}


void Surge::Transport::RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) {
    
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

Surge::Response* Surge::Transport::RtspTransaction(const RtspCommand* command, bool waitForResponse) {
    RtspTransaction(command, [&](Response *response) {
        m_rtspOutputQueue.AddItem(response);
    });
    
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

void Surge::Transport::RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) {
    DEBUG("Sending command to server");
    
    m_rtspResponseSubject.get_observable()
        .take(1)
        .subscribe([&](Response *response) {
            callback(response);
        }, [](std::exception_ptr error) {
            ERROR("BROKEN");
        });
    
    m_tcp->write(generateRtspDataPtr((char *)command->BytesPointer(), command->PointerLength()),
                 command->PointerLength());
}

std::unique_ptr<char[]> Surge::Transport::generateRtspDataPtr(char *data, size_t length) {
    char *dataCopy = new char[length];
    memcpy(dataCopy, data, length);
    return std::unique_ptr<char[]>(dataCopy);
}


void Surge::Transport::Run() {
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
