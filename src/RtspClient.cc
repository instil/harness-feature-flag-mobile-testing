#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Logging.h"
#include "Datetime.h"
#include "Url.h"

#include "H264Depacetizer.h"
#include "MP4VDepacketizer.h"

Surge::RtspClient::RtspClient(Surge::RtspClientDelegate *delegate) : m_delegate(delegate),
                                                                     m_processedFirstPayload(false),
                                                                     m_lastKeepAliveMs(0),
                                                                     m_keeepAliveIntervalInSeconds(60),
                                                                     m_sequenceNumber(1),
                                                                     m_url(""),
                                                                     m_session(""),
                                                                     m_socketHandler() { }

Surge::RtspClient::~RtspClient() {
    if (m_socketHandler.IsRunning()) {
        m_socketHandler.StopRunning();
    }
    if (m_thread.IsRunning()) {
        m_thread.Stop();
    }
}

Surge::DescribeResponse* Surge::RtspClient::Describe(const std::string url,
                                                     bool requires_auth,
                                                     const std::string user,
                                                     const std::string password) {
    m_url = url;
    SurgeUtil::Url url_model(url);
    
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    int retval = m_socketHandler.RtspTcpOpen(host, port);
    if (retval != 0) {
        ERROR("Failed to open [rtsp://" << host << ":" << port << "]");
        return nullptr;
    }

    m_socketHandler.StartRunning();
    if (requires_auth) {
        RtspCommandFactory::SetBasicAuthCredentials(user.c_str(), password.c_str());
    }

    RtspCommand* describe = RtspCommandFactory::DescribeRequest(url, GetNextSequenceNumber(), true);
    Response* raw_resp = m_socketHandler.RtspTransaction(describe, true);
    delete describe;
    
    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to DESCRIBE!");
        return nullptr;
    }
    
    DescribeResponse *resp = new DescribeResponse(raw_resp);
    delete raw_resp;

    return resp;
}

Surge::SetupResponse* Surge::RtspClient::Setup(const SessionDescription sessionDescription) {
    
    // control url is where we put any more requests to
    std::string setup_url = (sessionDescription.IsControlUrlComplete()) ?
        sessionDescription.GetControl():
        m_url + "/" + sessionDescription.GetControl();

    // this is the new url we need to use for all requests now
    m_url = setup_url;

    // set current palette
    m_currentPalette = sessionDescription;

    // new session = no processed payloads
    m_processedFirstPayload = false;
    
    RtspCommand* setup = RtspCommandFactory::SetupRequest(m_url, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(setup, true);
    delete setup;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to SETUP!");
        return nullptr;
    }

    SetupResponse* resp = new SetupResponse(raw_resp);
    delete raw_resp;

    if (resp->Ok()) {
        m_lastKeepAliveMs = SurgeUtil::CurrentTimeMilliseconds();
        m_keeepAliveIntervalInSeconds = resp->GetTimeout();
        m_session = resp->GetSession();

        DEBUG("RtspClient Session set to: " << m_session);
        DEBUG("RtspClient KeepAlive Interval set to: " << m_keeepAliveIntervalInSeconds);

        if (resp->IsInterleaved()) {
            m_socketHandler.SetRtpInterleavedChannel(resp->GetRtpInterleavedChannel());
            m_socketHandler.SetRtcpInterleavedChannel(resp->GetRtcpInterleavedChannel());

            DEBUG("Rtp Interleaved Channel set to: " << resp->GetRtpInterleavedChannel());
            DEBUG("Rtcp Interleaved Channel set to: " << resp->GetRtcpInterleavedChannel());
        }
        
        StartSession();
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Play(bool waitForResponse) {
    RtspCommand* play = RtspCommandFactory::PlayRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(play, true);
    delete play;

    bool received_response = raw_resp != nullptr;
    if (!received_response && waitForResponse) {
        ERROR("Failed to get response to PLAY!");
        return nullptr;
    }

    RtspResponse* resp = nullptr;
    if (waitForResponse) {
        resp = new RtspResponse(raw_resp);
        delete raw_resp;
    } else {
        resp = new RtspResponse(200, "");
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Pause() {
    RtspCommand* pause = RtspCommandFactory::PauseRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(pause, true);
    delete pause;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to PAUSE!");
        return nullptr;
    }

    RtspResponse* resp = new RtspResponse(raw_resp);
    delete raw_resp;
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Options() {    
    RtspCommand* options = RtspCommandFactory::OptionsRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(options, true);
    delete options;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to OPTIONS!");
        return nullptr;
    }

    RtspResponse* resp = new RtspResponse(raw_resp);
    delete raw_resp;

    return resp;
}

Surge::RtspResponse* Surge::RtspClient::Teardown(bool waitForResponse) {
    RtspCommand* teardown = RtspCommandFactory::TeardownRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(teardown, waitForResponse);
    delete teardown;

    bool received_response = raw_resp != nullptr;
    if (!received_response && waitForResponse) {
        ERROR("Failed to get response to TEARDOWN!");
        return nullptr;
    }
    
    RtspResponse* resp = nullptr;
    if (waitForResponse) {
        resp = new RtspResponse(raw_resp);
        delete raw_resp;
    } else {
        resp = new RtspResponse(200, "");
    }
    
    return resp;
}

Surge::RtspResponse* Surge::RtspClient::KeepAlive() {
    RtspCommand* keep_alive = RtspCommandFactory::KeepAliveRequest(m_url, m_session, GetNextSequenceNumber());
    Response* raw_resp = m_socketHandler.RtspTransaction(keep_alive, true);
    delete keep_alive;

    bool received_response = raw_resp != nullptr;
    if (!received_response) {
        ERROR("Failed to get response to Keep-Alive!");
        return nullptr;
    }

    RtspResponse* resp = new RtspResponse(raw_resp);
    delete raw_resp;
    
    return resp;
}

void Surge::RtspClient::StopClient() {
    RtspResponse* teardown_response = Teardown(false);
    delete teardown_response;
    
    if (m_socketHandler.IsRunning()) {
        m_socketHandler.StopRunning();
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
    
    while (true) {

        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({
                &m_thread.StopRequested(),
                &m_socketHandler.GetRtpPacketQueue()->GetNonEmptyEvent()
            },
            1000);

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, m_thread.StopRequested())) {
            DEBUG("RtspClient - Stop Requested.");
            break;
        }

        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents,
                                                     m_socketHandler.GetRtpPacketQueue()->GetNonEmptyEvent()))
        {
            // handle new rtp data here...
            RtpPacket* packet = m_socketHandler.GetRtpPacketQueue()->RemoveItem();
            ProcessRtpPacket(packet);
            delete packet;
        }

        uint64_t current_time_ms = SurgeUtil::CurrentTimeMilliseconds();
        int64_t delta = current_time_ms - m_lastKeepAliveMs;
        int64_t delta_seconds = delta / 1000;
        
        bool need_keep_alive = delta_seconds >= static_cast<int64_t>(m_keeepAliveIntervalInSeconds * 0.9);
        if (need_keep_alive) {
            DEBUG("Sending Keep Alive for session: " << m_url);

            RtspResponse* resp = KeepAlive();
            if (resp == nullptr) {
                // notify delegate via error dispatcher
                ERROR("Failed to get response to keep alive");
                NotifyDelegateTimeout();
            } else if (!resp->Ok()) {
                ERROR("Failed Keep-Alive request: " << resp->GetCode());
                delete resp;
                NotifyDelegateTimeout();
            } else {
                m_lastKeepAliveMs = SurgeUtil::CurrentTimeMilliseconds();
                delete resp;
            }
        }
    }
    INFO("Rtsp Client is Finished");
}

void Surge::RtspClient::ProcessRtpPacket(const RtpPacket* packet) {

    switch (m_currentPalette.GetType()) {
    case H264:
        ProcessH264Packet(packet);
        break;

    case JPEG:
        ProcessJPEGPacket(packet);
        break;

    case MP4V:
        ProcessMP4VPacket(packet);
        break;
        
    default:
        ERROR("Unhandled session type: " << m_currentPalette.GetType());
        return;
    }

    m_processedFirstPayload = true;
    
    if (!packet->IsMarked()) {
        return;
    }

    // add padding
#define PADDING_SIZE 32

    size_t current_frame_size = GetCurrentFrameSize();
    const unsigned char *current_frame = GetCurrentFrame();
    
    size_t padded_payload_size = current_frame_size + PADDING_SIZE;
    unsigned char *padded_payload = (unsigned char*)malloc(padded_payload_size);
    memset(padded_payload, 0, padded_payload_size);
    memcpy(padded_payload, current_frame, current_frame_size);

    // notify delegate of new payload
    NotifyDelegatePayload(padded_payload, padded_payload_size);

    free(padded_payload);

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

void Surge::RtspClient::ProcessJPEGPacket(const RtpPacket* packet) {
    
}
