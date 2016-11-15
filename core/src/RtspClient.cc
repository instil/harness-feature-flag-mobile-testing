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

#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Datetime.h"
#include "Url.h"

#include "InterleavedRtspTransport.h"
#include "UdpTransport.h"

#include "H264Depacketizer.h"
#include "MP4VDepacketizer.h"
#include "MJPEGDepacketizer.h"

using SurgeUtil::Constants::DEFAULT_NO_PACKET_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS;


Surge::RtspClient::RtspClient(Surge::IRtspClientDelegate * const delegate, bool forceInterleavedTransport) :
        m_delegate(delegate),
        m_noPacketTimeout(DEFAULT_NO_PACKET_TIMEOUT_MS),
        processedFirstPayload(false),
        m_lastKeepAliveMs(0),
        m_keeepAliveIntervalInSeconds(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS),
        m_sequenceNumber(1),
        startTimeSet(false),
        endTimeSet(false),
        factory(new SessionDescriptionFactory()){
            
    StartErrorDispatcher();

    if (forceInterleavedTransport) {
        m_transport = new InterleavedRtspTransport(nullptr);
    } else {
//        m_transport = new UdpTransport(nullptr);
    }
    m_transport->SetDelegate(this);
    frameBuffer = new std::vector<unsigned char>();
}

Surge::RtspClient::~RtspClient() {
    if (m_transport->IsRunning()) {
        m_transport->StopRunning();
    }
    
    if (m_thread.IsRunning()) {
        m_thread.Stop();
    }

    delete frameBuffer;
    delete depacketizer;
    delete m_transport;
    delete factory;
}

void Surge::RtspClient::Describe(const std::string& url,
                                 std::function<void(Surge::DescribeResponse*)> callback) {
    Describe(url, "", "", callback);
}

void Surge::RtspClient::Describe(const std::string& url,
                                                     const std::string& user,
                                                     const std::string& password,
                                                     std::function<void(Surge::DescribeResponse*)> callback) {
    m_url = url;
    m_isPlaying = false;

    SetupRtspConnection(url, [&](int result) {
        if (result != 0) {
            callback(nullptr);
        }
        
        if (user.length() > 0 && password.length() > 0) {
            RtspCommandFactory::SetBasicAuthCredentials(user.c_str(), password.c_str());
        }
        
        RtspCommand* describe;
        if (startTimeSet) {
            describe = RtspCommandFactory::DescribeRequest(url, GetNextSequenceNumber(), startDate);
        }
        else {
            describe = RtspCommandFactory::DescribeRequest(url, GetNextSequenceNumber());
        }
        
        Response* raw_resp = m_transport->RtspTransaction(describe, true);
        delete describe;
        
        bool received_response = raw_resp != nullptr;
        if (!received_response) {
            ERROR("Failed to get response to DESCRIBE!");
            callback(nullptr);
        }
        
        
        DescribeResponse *resp = nullptr;
        try {
            resp = new DescribeResponse(raw_resp, factory);
        }
        catch (const std::exception& e) {
            ERROR("Invalid DescribeResponse: " << e.what());
            resp = nullptr;
        }
        
        if (!resp->Ok()) {
            NotifyDelegateTimeout();
        }
        
        delete raw_resp;
        
        callback(resp);
    });
}

Surge::SetupResponse* Surge::RtspClient::Setup(const SessionDescription& sessionDescription, bool serverAllowsAggregate) {
    
    // control url is where we put any more requests to
    std::string setup_url = (sessionDescription.IsControlUrlComplete()) ?
        sessionDescription.GetControl():
        m_url + "/" + sessionDescription.GetControl();

    // Gstreamer doesnt like using the control url for subsequent rtsp requests post setup
    // only applicable in non complete control url's.
    if (serverAllowsAggregate && !sessionDescription.IsControlUrlComplete()) {
        // this is the new url we need to use for all requests now
        m_url = setup_url;
    }
    else if (sessionDescription.IsControlUrlComplete()) {
        m_url = setup_url;
    }

    // set current palette
    m_sessionDescription = sessionDescription;

    CreateDepacketizer();

    // new session = no processed payloads
    processedFirstPayload = false;
    
    RtspCommand* setup = RtspCommandFactory::SetupRequest(setup_url, GetNextSequenceNumber(), m_transport);
    Response* raw_resp = m_transport->RtspTransaction(setup, true);
    delete setup;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to SETUP!");
        return nullptr;
    }

    SetupResponse* resp = nullptr;
    try {
        resp = new SetupResponse(raw_resp);
    }
    catch (const std::exception& e) {
        ERROR("Invalid SetupResponse: " << e.what());
        resp = nullptr;
    }
    
    delete raw_resp;

    if (resp != nullptr && resp->Ok()) {
        m_lastKeepAliveMs = SurgeUtil::currentTimeMilliseconds();
        m_keeepAliveIntervalInSeconds = resp->GetTimeoutSeconds();
        m_session = resp->GetSession();

        DEBUG("RtspClient Session set to: " << m_session);
        DEBUG("RtspClient KeepAlive Interval set to: " << m_keeepAliveIntervalInSeconds);

        if (m_transport->IsInterleavedTransport() && resp->IsInterleaved()) {
            ((InterleavedRtspTransport*)m_transport)->
                SetRtpInterleavedChannel(resp->GetRtpInterleavedChannel());
            ((InterleavedRtspTransport*)m_transport)->
                SetRtcpInterleavedChannel(resp->GetRtcpInterleavedChannel());

            DEBUG("Rtp Interleaved Channel set to: " << resp->GetRtpInterleavedChannel());
            DEBUG("Rtcp Interleaved Channel set to: " << resp->GetRtcpInterleavedChannel());
        }
        
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Play(bool waitForResponse) {
    RtspCommand* play;
    
    if (endTimeSet) {
        play = RtspCommandFactory::PlayRequest(m_url, m_session, GetNextSequenceNumber(), startDate, endDate);
    }
    else if (startTimeSet) {
        play = RtspCommandFactory::PlayRequest(m_url, m_session, GetNextSequenceNumber(), startDate);
    }
    else {
        play = RtspCommandFactory::PlayRequest(m_url, m_session, GetNextSequenceNumber());
    }
    
    Response* raw_resp = m_transport->RtspTransaction(play, true);
    delete play;

    bool received_response = raw_resp != nullptr;
    if (!received_response && waitForResponse) {
        ERROR("Failed to get response to PLAY!");
        return nullptr;
    }

    RtspResponse* resp = nullptr;
    if (waitForResponse) {
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid PlayResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;
    } else {
        resp = new RtspResponse(200, "");
    }

    m_isPlaying = resp != nullptr && resp->Ok();
    StartSession();
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Pause() {
    RtspCommand* pause = RtspCommandFactory::PauseRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_transport->RtspTransaction(pause, true);
    delete pause;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to PAUSE!");
        return nullptr;
    }

    RtspResponse* resp = nullptr;
    try {
        resp = new RtspResponse(raw_resp);
    }
    catch (const std::exception& e) {
        ERROR("Invalid PauseResponse: " << e.what());
        resp = nullptr;
    }
    delete raw_resp;

    m_isPlaying = false;
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Options() {
    RtspCommand* options = RtspCommandFactory::OptionsRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_transport->RtspTransaction(options, true);
    delete options;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to OPTIONS!");
        return nullptr;
    }

    RtspResponse* resp = nullptr;
    try {
        resp = new RtspResponse(raw_resp);
    }
    catch (const std::exception& e) {
        ERROR("Invalid OptionsResponse: " << e.what());
        resp = nullptr;
    }
    delete raw_resp;

    return resp;
}

void Surge::RtspClient::Options(const std::string& url,
                                std::function<void(Surge::RtspResponse*)> callback) {
    
    SetupRtspConnection(url, [&](int result) {
        if (result != 0) {
            callback(nullptr);
        }
        
        RtspCommand* options = RtspCommandFactory::OptionsRequest(url, m_session, GetNextSequenceNumber());
        Response* raw_resp = m_transport->RtspTransaction(options, true);
        delete options;
        
        bool received_response = raw_resp != nullptr;
        if (!received_response) {
            ERROR("Failed to get response to OPTIONS!");
            callback(nullptr);
        }
        
        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid OptionsResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;
        
        callback(resp);
    });
    
}

Surge::RtspResponse* Surge::RtspClient::Teardown(bool waitForResponse) {
    RtspCommand* teardown = RtspCommandFactory::TeardownRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_transport->RtspTransaction(teardown, waitForResponse);
    delete teardown;

    bool received_response = raw_resp != nullptr;
    if (!received_response && waitForResponse) {
        ERROR("Failed to get response to TEARDOWN!");
        return nullptr;
    }
    
    RtspResponse* resp = nullptr;
    if (waitForResponse) {
        try {
            resp = new RtspResponse(raw_resp);
        }
        catch (const std::exception& e) {
            ERROR("Invalid TeardownResponse: " << e.what());
            resp = nullptr;
        }
        delete raw_resp;
    } else {
        resp = new RtspResponse(200, "");
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::KeepAlive() {
    RtspCommand* keep_alive = RtspCommandFactory::KeepAliveRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_transport->RtspTransaction(keep_alive, true);
    delete keep_alive;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to Keep-Alive!");
        return nullptr;
    }

    RtspResponse* resp = nullptr;
    try {
        resp = new RtspResponse(raw_resp);
    } catch (const std::exception& e) {
        ERROR("Invalid KeepaliveResponse: " << e.what());
        resp = nullptr;
    }
    
    delete raw_resp;
    
    return resp;
}

void Surge::RtspClient::StopClient() {
    Abort();
    
    if (m_transport->IsRunning()) {

        // non empty session token we should teardown
        if (!m_session.empty()) {
            RtspResponse* teardown_response = Teardown(false);
            if (teardown_response != nullptr) {
                delete teardown_response;
            }
        }
        
        m_transport->StopRunning();
    }
    
    if (m_thread.IsRunning()) {
        m_thread.Stop();
    }
}

void Surge::RtspClient::StopStream() {
    m_transport->StopRunning();
}

void Surge::RtspClient::StartSession() {
    if (m_thread.IsRunning()) {
        return;
    }

    m_thread.Execute(*this);
}

void Surge::RtspClient::Run() {

    uint64_t time_last_packet_was_processed = SurgeUtil::currentTimeMilliseconds();

    auto rtp_packet_obs = m_transport->GetRtpPacketObservable().subscribe(
        [&](RtpPacket* packet) {
            if (packet != nullptr) {
                ProcessRtpPacket(packet);
                
                delete packet;
                
                time_last_packet_was_processed = SurgeUtil::currentTimeMilliseconds();
            }
        },
        [](std::exception_ptr){
            // TODO
        }
    );
    
    while (true) {
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({m_thread.StopRequested()}, 1000);        

        // STOP
        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, *(m_thread.StopRequested()))) {
            DEBUG("RtspClient - Stop Requested.");
            break;
        }

        // TIMEOUT
        int64_t timeout_delta_ms = SurgeUtil::currentTimeMilliseconds() - time_last_packet_was_processed;
        bool client_did_timeout = m_isPlaying && timeout_delta_ms >= m_noPacketTimeout;
        if (client_did_timeout) {
            ERROR("No processed packets in the last " << timeout_delta_ms << "(ms). Issueing timeout signal.");
            NotifyDelegateTimeout();
            break;
        }

        // KEEP ALIVE        
        int64_t delta = SurgeUtil::currentTimeMilliseconds() - m_lastKeepAliveMs;
        int64_t delta_seconds = delta / 1000;
        bool need_keep_alive = delta_seconds >= static_cast<int64_t>(m_keeepAliveIntervalInSeconds * 0.9);
        if (need_keep_alive) {
            DEBUG("Sending Keep Alive for session: " << m_url);

            RtspResponse* resp = KeepAlive();
            if (resp == nullptr) {
                // notify delegate via error dispatcher
                ERROR("Failed to get response to keep alive");
                NotifyDelegateTimeout();
                break;
            } else if (!resp->Ok()) {
                ERROR("Failed Keep-Alive request: " << resp->GetCode());
                delete resp;
                NotifyDelegateTimeout();
                break;
            } else {
                m_lastKeepAliveMs = SurgeUtil::currentTimeMilliseconds();
                delete resp;
            }
        }
    }    
    INFO("Rtsp Client is Finished");
    rtp_packet_obs.unsubscribe();
}

void Surge::RtspClient::ProcessRtpPacket(const RtpPacket* packet) {
    if (depacketizer == nullptr) {
        WARNING("Received RTP packet but no depacketizer available");
        return;
    }

    depacketizer->ProcessPacket(packet, !processedFirstPayload);
    processedFirstPayload = true;
    
    if (packet->HasExtension()) {
        NotifyDelegateOfExtendedRtpHeader(packet->GetExtensionData(),
                                          packet->GetExtensionLength());
    }
    
    if (!packet->IsMarked()) {
        return;
    }

    NotifyDelegateOfAvailableFrame();
    ClearFrameBuffer();
}

void Surge::RtspClient::CreateDepacketizer() {
    switch (m_sessionDescription.GetType()) {
        case H264:
            depacketizer = new H264Depacketizer(m_sessionDescription, frameBuffer);
            break;
        case MP4V:
            depacketizer = new MP4VDepacketizer(m_sessionDescription, frameBuffer);
            break;
        case MJPEG:
            depacketizer = new MJPEGDepacketizer(m_sessionDescription, frameBuffer);
            break;
        default:
            ERROR("Content type " << m_sessionDescription.GetType() << " not currently supported");
            return;
    }
}

void Surge::RtspClient::SetupRtspConnection(const std::string& url, std::function<void(int)> callback) {
    if (m_transport->IsRunning()) {
        WARNING("Trying to setup RTSP connection on already running transport - resetting connection.");
        m_transport->StopRunning();
    }

    m_abortWait.Reset();
    
    SurgeUtil::Url url_model(url);
    
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    m_transport->RtspTcpOpen(host, port, [&](int result) {
        if (result == 0) {
            m_transport->StartRunning();
        } else {
            ERROR("Did not start the Transport thread.");
        }
        
        callback(result);
    });
}
