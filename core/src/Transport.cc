// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "Transport.h"

using SurgeUtil::Constants::DEFAULT_TRANSACTION_TIMEOUT_MS;


Surge::Transport::Transport(TransportDelegate * const transportDelegate, ISocketHandlerDelegate *delegate) : m_rtspTimeoutTime(uvw::TimerHandle::Time(DEFAULT_TRANSACTION_TIMEOUT_MS)), m_threadRunning(false), executingLibuvCommand(false), m_delegate(delegate), transportDelegate(transportDelegate)
{ }

Surge::Transport::~Transport() {
    StopRunning();
    
    if (m_tcp != nullptr) {
        m_tcp->close();
    }
    
    if (m_loop != nullptr) {
        m_loop->stop();
    }
}

/*  Thread handling  */

void Surge::Transport::StartRunning() {
    DEBUG("Starting transport");

    if (IsRunning()) {
        return;
    }

    if (m_receivedBuffer.size() > 0) {
        DEBUG("Transport packet buffer clear required.");
        m_receivedBuffer.clear();
    }
    
    m_threadRunning = true;
    m_thread.Execute(*this);
}

void Surge::Transport::StopRunning() {
    DEBUG("Stopping transport");

    if (!IsRunning()) {
        return;
    }
    m_threadRunning = false;
    m_libuvCloser->send();

    if (m_thread.IsRunning()) {
        m_thread.WaitUntilStopped();
    }
}
/*  Network  */

void Surge::Transport::RtspTcpOpen(SurgeUtil::Url &url, std::function<void(int)> callback) {
    if (!m_threadRunning) {
        if (m_loop == nullptr) {
            m_loop = uvw::Loop::create();
        }
        m_tcp = m_loop->resource<uvw::TcpHandle>();
        m_timer = m_loop->resource<uvw::TimerHandle>();
        m_libuvCloser = m_loop->resource<uvw::AsyncHandle>();
    }

    m_streamIp = ResolveHostnameToIP(url.GetHost(), url.GetPort());

    m_tcp->once<uvw::ErrorEvent>([&, callback](const uvw::ErrorEvent &error, uvw::TcpHandle &tcp) {
        ERROR("Error occured on connect");
        callback(error.code());
    });


    m_tcp->once<uvw::ConnectEvent>([&, callback](const uvw::ConnectEvent &connectEvent, uvw::TcpHandle &tcp) {
        AttachCallbacksToLibuv();

        INFO("Connected");
        callback(0);
    });

    INFO("Starting the Transport thread");
    StartRunning();

    INFO("Connecting to TCP port");
    SafeRunLibuvCommand([&]() {
        if (SurgeUtil::Url::IpIsIPv6(m_streamIp)) {
            m_tcp->connect<uvw::IPv6>(m_streamIp, url.GetPort());
        } else {
            m_tcp->connect<uvw::IPv4>(m_streamIp, url.GetPort());
        }
    });
}

std::string Surge::Transport::ResolveHostnameToIP(const std::string& host, const int port) {
    char ip[128] = { 0 };
    
    auto request = m_loop->resource<uvw::GetAddrInfoReq>();
    auto result = request->nodeAddrInfoSync(host);

    if (result.second->ai_family == AF_INET) {
        uv_ip4_name((struct sockaddr_in*) result.second->ai_addr, ip, sizeof(ip));

    } else if (result.second->ai_family == AF_INET6) {
        uv_ip6_name((struct sockaddr_in6*)result.second->ai_addr, ip, sizeof(ip));
    }
    
    return std::string(ip);
}

void Surge::Transport::RtspTransaction(const RtspCommand* command, std::function<void(Response*)> callback) {
    DEBUG("Sending command to server");
    
    rtspCallback = callback;
  
    SafeRunLibuvCommand([&]() {
        m_tcp->write(GenerateRtspDataPtr((char *)command->BytesPointer(), command->PointerLength()),
                   command->PointerLength());
        m_tcp->read();
    });
}

void Surge::Transport::ArbitraryDataTransaction(const char *data, const size_t length) {
    DEBUG("Sending arbitrary data down the socket");

    SafeRunLibuvCommand([&]() {
        m_tcp->write(GenerateRtspDataPtr((char *) data, length), length);
        this->StartRtspTimer();
        m_tcp->read();
    });
}


void Surge::Transport::SafeRunLibuvCommand(std::function<void()> commandsToRun) {
    executingLibuvCommand = false;
    m_loop->stop();
    commandsToRun();
    executingLibuvCommand = true;
}

void Surge::Transport::StartRtspTimer() {
    DEBUG("Started RTSP transaction timeout timer.");
    m_timer->start(m_rtspTimeoutTime, uvw::TimerHandle::Time(0));
}

void Surge::Transport::StopRtspTimer() {
    m_timer->stop();
}

std::unique_ptr<char[]> Surge::Transport::GenerateRtspDataPtr(char *data, size_t length) {
    char *dataCopy = new char[length];
    memcpy(dataCopy, data, length);
    return std::unique_ptr<char[]>(dataCopy);
}

void Surge::Transport::AttachCallbacksToLibuv() {
    m_tcp->clear();
    m_timer->clear();
    m_libuvCloser->clear();

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

    m_tcp->on<uvw::WriteEvent>([this](const uvw::WriteEvent &writeEvent, uvw::TcpHandle &tcp) {
        DEBUG("Sent request to stream, wait for a response");
    });

    m_tcp->on<uvw::DataEvent>([this](const uvw::DataEvent &dataEvent, uvw::TcpHandle &tcp) {
//        DEBUG("Data received, handing received data");
        RtspHandleReceive(dataEvent.data.get(), dataEvent.length);
    });

    m_tcp->on<uvw::EndEvent>([this](const uvw::EndEvent &endEvent, uvw::TcpHandle &tcp) {
        ERROR("Loop ended prematurely, closing TCP port");
        m_threadRunning = false;

        NotifyDelegateOfReadFailure();
    });

    m_timer->on<uvw::TimerEvent>([this](const uvw::TimerEvent &timerEvent, uvw::TimerHandle &timer) {
        INFO("RTSP timeout triggered, cancelling RTSP request.");
        rtspCallback(nullptr);
    });

    m_libuvCloser->on<uvw::AsyncEvent>([this](const uvw::AsyncEvent &asyncEvent, uvw::AsyncHandle &asyncHandle) {
        DEBUG("Transport thread: triggering stop request.");
        m_loop->stop();
    });
}

void Surge::Transport::Run() {
    while (m_threadRunning) {
        if (executingLibuvCommand) {
            m_loop->run();
        }
    }

    // After closing a handle, run the libuv loop once to allow libuv to call the
    // associated close event callback and clean up the resource.
    INFO("Closing transport");
    
    m_timer->close();
    m_loop->run<uvw::Loop::Mode::NOWAIT>();
    m_tcp->close();
    m_loop->run<uvw::Loop::Mode::NOWAIT>();

    m_loop->clear();
    m_loop->run<uvw::Loop::Mode::NOWAIT>();

    INFO("Transport stopped");
}

/*  Packet handling  */

void Surge::Transport::RtspHandleReceive(const char* buffer, size_t size) {
    if (!m_threadRunning) {
        return;
    }
    
    AppendDataToBuffer(buffer, size);
    
    do {
        if (BufferContainsAnnouncePacket()) {
            NotifyDelegateOfAnnounce();
            break;
        }
        else if (BufferContainsRedirectPacket()) {
            NotifyDelegateOfRedirect();
            break;
        }
        else if (BufferContainsRtspPacket()) {
            bool result = HandleRtspPacket();
            if (!result) {
                break;
            }
        }
        else {
            bool result = HandlePacket(buffer, size);
            if (!result) {
                break;
            }
        }
    } while (0 < m_receivedBuffer.size() && m_threadRunning);
}

bool Surge::Transport::HandleRtspPacket() {
    std::string string_response;
    string_response.resize(m_receivedBuffer.size());
    std::copy(m_receivedBuffer.begin(), m_receivedBuffer.end(), string_response.begin());
    
    size_t body_pos;
    if ((body_pos = string_response.find("\r\n\r\n")) != std::string::npos) {
        size_t pos = string_response.find("Content-Length:");
        size_t content_length = (pos != std::string::npos) ?
        static_cast<size_t>(atoi(&string_response[pos + 16])) :
        0;
        
        size_t headers_length = body_pos;

        const unsigned char *rtsp_buffer = &(m_receivedBuffer[0]);
        size_t rtsp_buffer_length = content_length + 4 + headers_length;
        
        if (m_receivedBuffer.size() < rtsp_buffer_length) {
            return false;
        }
        
        if (rtspCallback != nullptr) {
            StopRtspTimer();
            rtspCallback(new Response(rtsp_buffer, rtsp_buffer_length));
        }
        
        RemoveDataFromStartOfBuffer(rtsp_buffer_length);
    } else {
        return false;
    }
    
    return true;
}

void Surge::Transport::AppendDataToBuffer(const char* buffer, size_t size) {
    if (size > 0) {
        // Append received data to 'response'.
        size_t old_size = m_receivedBuffer.size();
        if (old_size < (old_size + size)) {
            m_receivedBuffer.resize(old_size + size);
        }
        copy(buffer, buffer + size, m_receivedBuffer.begin() + old_size);
    }
}

void Surge::Transport::RemoveDataFromStartOfBuffer(size_t count) {
    m_receivedBuffer.erase(m_receivedBuffer.begin(), m_receivedBuffer.begin() + count);
}
