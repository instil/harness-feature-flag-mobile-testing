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
                                                        m_delegate(delegate)
{ }

Surge::Transport::~Transport() {
    StopRunning();
    
    m_tcp->close();
    m_loop->stop();
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
    m_loop->stop();
    if (m_thread.IsRunning()) {
        m_thread.WaitUntilStopped();
    }
}


void Surge::Transport::RtspTcpOpen(const std::string& host, int port, std::function<void(int)> callback) {
    
    if (!m_running) {
        m_loop = uvw::Loop::create();
        m_tcp = m_loop->resource<uvw::TcpHandle>();
    }
    
    m_tcp->once<uvw::ErrorEvent>([&](const uvw::ErrorEvent &error, uvw::TcpHandle &tcp) {
        ERROR("Error occured on connect");
        callback(error.code());
    });
    
    m_tcp->once<uvw::ConnectEvent>([&](const uvw::ConnectEvent &connectEvent, uvw::TcpHandle &tcp) {
        INFO("Connected");
        callback(0);
    });
    
    INFO("Connecting to TCP port");
    m_tcp->connect(host, port);
    m_loop->run<uvw::Loop::Mode::ONCE>();
}

void Surge::Transport::RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) {
    DEBUG("Sending command to server");
    
    rtspCallback = callback;


    executingRtspCommand = false;
    m_loop->stop();
    m_tcp->write(generateRtspDataPtr((char *) command->BytesPointer(), command->PointerLength()),
                 command->PointerLength());
    m_tcp->read();
    executingRtspCommand = true;
}

std::unique_ptr<char[]> Surge::Transport::generateRtspDataPtr(char *data, size_t length) {
    char *dataCopy = new char[length];
    memcpy(dataCopy, data, length);
    return std::unique_ptr<char[]>(dataCopy);
}


void Surge::Transport::Run() {

    m_tcp->clear();

    m_tcp->on<uvw::ErrorEvent>([this](const uvw::ErrorEvent &error, uvw::TcpHandle &tcp) {
        ERROR("ERROR");
        NotifyDelegateOfReadFailure();
    });
    
    m_tcp->on<uvw::ListenEvent>([](const uvw::ListenEvent &listenEvent, uvw::TcpHandle &tcp) {
        INFO("Listening");
    });
    
    m_tcp->on<uvw::ShutdownEvent>([](const uvw::ShutdownEvent &shutdownEvent, uvw::TcpHandle &tcp) {
        ERROR("Shutdown");
    });
    
    m_tcp->on<uvw::WriteEvent>([](const uvw::WriteEvent &writeEvent, uvw::TcpHandle &tcp) {
        DEBUG("Sent request to stream, wait for a response");
    });
    
    m_tcp->on<uvw::DataEvent>([this](const uvw::DataEvent &dataEvent, uvw::TcpHandle &tcp) {
//        DEBUG("Data received, handing received data");
        RtspHandleReceive(dataEvent.data.get(), dataEvent.length);
    });
    
    m_tcp->on<uvw::EndEvent>([this](const uvw::EndEvent &endEvent, uvw::TcpHandle &tcp) {
        ERROR("Loop ended prematurely, closing TCP port");
        m_running = false;
        
        NotifyDelegateOfReadFailure();
    });

    while (m_running) {
        if (executingRtspCommand) {
            m_loop->run();
        }
    }
    
    
    ERROR("Closing transport");
    m_loop->close();
    m_tcp->close();
}
