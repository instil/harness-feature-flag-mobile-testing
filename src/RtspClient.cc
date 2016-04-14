#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Logging.h"
#include "Datetime.h"
#include "Url.h"
#include "Constants.h"

#include "H264Depacetizer.h"
#include "MP4VDepacketizer.h"
#include "MJPEGDepacketizer.h"

using SurgeUtil::Constants::DEFAULT_NO_PACKET_TIMEOUT_MS;
using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS;


Surge::RtspClient::RtspClient(Surge::RtspClientDelegate * const delegate,
                              Surge::ITransportInterface * const transport)
    : m_delegate(delegate),
      m_noPacketTimeout(DEFAULT_NO_PACKET_TIMEOUT_MS),
      m_processedFirstPayload(false),
      m_lastKeepAliveMs(0),
      m_keeepAliveIntervalInSeconds(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS),
      m_sequenceNumber(1),
      m_transport(transport)
{
    m_transport->SetDelegate(this);
}

Surge::RtspClient::RtspClient(Surge::RtspClientDelegate * const delegate)
    : m_delegate(delegate),
      m_noPacketTimeout(DEFAULT_NO_PACKET_TIMEOUT_MS),
      m_processedFirstPayload(false),
      m_lastKeepAliveMs(0),
      m_keeepAliveIntervalInSeconds(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS),
      m_sequenceNumber(1)
{
    // default interleaved is safest to avoid NAT issues.
    //   NOTE: Can fail when some servers don't support interleaved mode
    m_transport = new InterleavedRtspTransport(this);
}

Surge::RtspClient::~RtspClient() {
    if (m_transport->IsRunning()) {
        m_transport->StopRunning();
    }
    
    if (m_thread.IsRunning()) {
        m_thread.Stop();
    }

    delete m_transport;
}

Surge::DescribeResponse* Surge::RtspClient::Describe(const std::string& url,
                                                     bool requires_auth,
                                                     const std::string& user,
                                                     const std::string& password) {
    m_url = url;
    m_isPlaying = false;

    int retval = SetupRtspConnection(url);
    if (retval != 0) {
        return nullptr;
    }
    
    if (requires_auth) {
        RtspCommandFactory::SetBasicAuthCredentials(user.c_str(), password.c_str());
    }

    RtspCommand* describe = RtspCommandFactory::DescribeRequest(url, GetNextSequenceNumber(), true);
    Response* raw_resp = m_transport->RtspTransaction(describe, true);
    delete describe;
    
    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to DESCRIBE!");
        return nullptr;
    }

    
    DescribeResponse *resp = nullptr;
    try {
        resp = new DescribeResponse(raw_resp);
    }
    catch (const std::exception& e) {
        ERROR("Invalid DescribeResponse: " << e.what());
        resp = nullptr;
    }
    
    delete raw_resp;

    return resp;
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
    m_currentPalette = sessionDescription;

    // new session = no processed payloads
    m_processedFirstPayload = false;
    
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
        m_lastKeepAliveMs = SurgeUtil::CurrentTimeMilliseconds();
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
        
        StartSession();
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Play(bool waitForResponse) {
    RtspCommand* play = RtspCommandFactory::PlayRequest(m_url, m_session, GetNextSequenceNumber());
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

Surge::RtspResponse* Surge::RtspClient::Options(const std::string& url) {
    int retval = SetupRtspConnection(url);
    if (retval != 0) {
        return nullptr;
    }
    
    RtspCommand* options = RtspCommandFactory::OptionsRequest(url, m_session, GetNextSequenceNumber());
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

void Surge::RtspClient::StartSession() {
    if (m_thread.IsRunning()) {
        return;
    }

    m_thread.Execute(*this);
}

void Surge::RtspClient::Run() {

    uint64_t time_last_packet_was_processed = SurgeUtil::CurrentTimeMilliseconds();

    auto rtp_packet_obs = m_transport->GetRtpPacketObservable().subscribe(
        [&](RtpPacket* packet) {
            if (packet != nullptr) {
                ProcessRtpPacket(packet);
                
                delete packet;
                
                time_last_packet_was_processed = SurgeUtil::CurrentTimeMilliseconds();
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
        int64_t timeout_delta_ms = SurgeUtil::CurrentTimeMilliseconds() - time_last_packet_was_processed;
        bool client_did_timeout = m_isPlaying && timeout_delta_ms >= m_noPacketTimeout;
        if (client_did_timeout) {
            ERROR("No processed packets in the last " << timeout_delta_ms << "(ms). Issueing timeout signal.");
            NotifyDelegateTimeout();
            break;
        }

        // KEEP ALIVE        
        int64_t delta = SurgeUtil::CurrentTimeMilliseconds() - m_lastKeepAliveMs;
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
                m_lastKeepAliveMs = SurgeUtil::CurrentTimeMilliseconds();
                delete resp;
            }
        }
    }    
    INFO("Rtsp Client is Finished");
}

void Surge::RtspClient::ProcessRtpPacket(const RtpPacket* packet) {

    SurgeUtil::MutexLocker lock(m_mutex);
    
    switch (m_currentPalette.GetType()) {
    case H264:
        ProcessH264Packet(packet);
        break;

    case MP4V:
        ProcessMP4VPacket(packet);
        break;

    case MJPEG:
        ProcessMJPEGPacket(packet);
        break;
        
    default:
        ERROR("Unhandled session type: " << m_currentPalette.GetType());
        return;
    }

    m_processedFirstPayload = true;
    
    if (!packet->IsMarked()) {
        return;
    }

    size_t current_frame_size = GetCurrentFrameSize();
    const unsigned char *current_frame = GetCurrentFrame();

    // notify delegate of new payload
    NotifyDelegatePayload(current_frame, current_frame_size);

    // reset
    ResetCurrentPayload();
}

void Surge::RtspClient::ProcessH264Packet(const RtpPacket* packet) {
    H264Depacketizer depacketizer(&m_currentPalette, packet, IsFirstPayload());

    const unsigned char *payload = depacketizer.PayloadBytes();
    size_t payload_size = depacketizer.PayloadLength();

    AppendPayloadToCurrentFrame(payload, payload_size);
}

void Surge::RtspClient::ProcessMP4VPacket(const RtpPacket* packet) {
    MP4VDepacketizer depacketizer(&m_currentPalette, packet, IsFirstPayload());

    const unsigned char *payload = depacketizer.PayloadBytes();
    size_t payload_size = depacketizer.PayloadLength();

    AppendPayloadToCurrentFrame(payload, payload_size);
}

void Surge::RtspClient::ProcessMJPEGPacket(const RtpPacket* packet) {
    MJPEGDepacketizer depacketizer(&m_currentPalette, packet, IsFirstPayload());

    depacketizer.AddToFrame(&m_currentFrame);
}

int Surge::RtspClient::SetupRtspConnection(const std::string& url) {
    if (m_transport->IsRunning()) {
        WARNING("Trying to setup RTSP connection on already running transport - ignoring request.");
        return 0;
    }

    m_abortWait.Reset();
    
    SurgeUtil::Url url_model(url);
    
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    int retval = m_transport->RtspTcpOpen(host, port, m_abortWait);

    if (retval == 0) {
        m_transport->StartRunning();
    }
    
    return retval;
}
