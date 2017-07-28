// Copyright (c) 2017 Instil Software.
//
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

#include "RtspClient.h"
#include "RtspCommandFactory.h"
#include "Datetime.h"
#include "Url.h"

#include "InterleavedRtspTransport.h"
#include "UdpTransport.h"

#include "H264Depacketizer.h"
#include "MP4VDepacketizer.h"
#include "MJPEGDepacketizer.h"

using SurgeUtil::Constants::DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS;


Surge::RtspClient::RtspClient(Surge::IRtspClientDelegate * const delegate, bool useInterleavedTcpTransport) :
        m_delegate(delegate),
        processedFirstPayload(false),
        m_lastKeepAliveMs(0),
        m_keeepAliveIntervalInSeconds(DEFAULT_KEEP_ALIVE_INTERVAL_SECONDS),
        m_sequenceNumber(1),
        m_timeout(SurgeUtil::Constants::DEFAULT_NO_PACKET_TIMEOUT_MS),
        startTimeSet(false),
        endTimeSet(false),
        depacketizer(nullptr),
        factory(new SessionDescriptionFactory()){
            
    StartErrorDispatcher();

    if (useInterleavedTcpTransport) {
        m_transport = new InterleavedRtspTransport(nullptr);
    } else {
        m_transport = new UdpTransport(nullptr);
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
    delete m_transport;
    delete factory;

    if (depacketizer != nullptr) {
        delete depacketizer;
    }
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
        INFO("Sending DESCRIBE request");
        
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

        m_transport->RtspTransaction(describe, [=](Response *raw_resp) {
            INFO("Received DESCRIBE response");
            
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
                ERROR("DESCRIBE command failed");
                ERROR(raw_resp->StringDump());
            }
            
            delete raw_resp;
            
            callback(resp);
        });
        
        delete describe;
    });
}

void Surge::RtspClient::Setup(const SessionDescription& sessionDescription,
                              bool serverAllowsAggregate,
                              std::function<void(Surge::SetupResponse*)> callback) {

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
    m_transport->RtspTransaction(setup, [=](Response *raw_resp) {
        bool received_response = raw_resp != nullptr;
        if (!received_response) {
            ERROR("Failed to get response to SETUP!");
            callback(nullptr);
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
        } else {
            ERROR("SETUP command failed");
            ERROR(raw_resp->StringDump());
        }

        callback(resp);
    });
    
    delete setup;
}

void Surge::RtspClient::Play(bool waitForResponse,
                             std::function<void(Surge::RtspResponse*)> callback) {
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

    m_transport->RtspTransaction(play, [=](Response *raw_resp) {
        bool received_response = raw_resp != nullptr;
        if (!received_response && waitForResponse) {
            ERROR("Failed to get response to PLAY!");
            callback(nullptr);
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

        if (!resp->Ok()) {
            ERROR("PLAY command failed");
            ERROR(raw_resp->StringDump());
        }

        StartSession();
        
        callback(resp);
    });
    
    delete play;
}

void Surge::RtspClient::Pause(std::function<void(Surge::RtspResponse*)> callback) {
    RtspCommand* pause = RtspCommandFactory::PauseRequest(m_url, m_session, GetNextSequenceNumber());
    m_transport->RtspTransaction(pause, [=](Response *raw_resp) {
        bool received_response = raw_resp != nullptr;
        if (!received_response) {
            ERROR("Failed to get response to PAUSE!");
            
            if (callback != NULL) {
                callback(nullptr);
            }
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
        
        if (callback != NULL) {
            callback(resp);
        }
    });
    
    delete pause;
}

void Surge::RtspClient::Options(std::function<void(Surge::RtspResponse*)> callback) {
    RtspCommand* options = RtspCommandFactory::OptionsRequest(m_url, m_session, GetNextSequenceNumber());
    m_transport->RtspTransaction(options, [&](Response *raw_resp) {
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
     
    delete options;
}

void Surge::RtspClient::Options(const std::string& url,
                                std::function<void(Surge::RtspResponse*)> callback) {
    
    SetupRtspConnection(url, [&](int result) {
        if (result != 0) {
            callback(nullptr);
        }
        
        RtspCommand* options = RtspCommandFactory::OptionsRequest(url, m_session, GetNextSequenceNumber());
        m_transport->RtspTransaction(options, [&](Response *raw_resp) {
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
        delete options;
    });
    
}

void Surge::RtspClient::Teardown(std::function<void(Surge::RtspResponse*)> callback,
                                 bool waitForResponse) {
    RtspCommand* teardown = RtspCommandFactory::TeardownRequest(m_url, m_session, GetNextSequenceNumber());
    m_transport->RtspTransaction(teardown, [&](Response *raw_resp) {
        bool received_response = raw_resp != nullptr;
        if (!received_response && waitForResponse) {
            ERROR("Failed to get response to TEARDOWN!");
            if (callback) {
                callback(nullptr);
            }
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
        
        if (callback) {
            callback(resp);
        } else {
            delete resp;
        }
    });
    
    delete teardown;
}

void Surge::RtspClient::KeepAlive(std::function<void(Surge::RtspResponse*)> callback) {
    RtspCommand* keep_alive = RtspCommandFactory::KeepAliveRequest(m_url, m_session, GetNextSequenceNumber());
    m_transport->RtspTransaction(keep_alive, [&](Response *raw_resp) {
        bool received_response = raw_resp != nullptr;
        if (!received_response) {
            ERROR("Failed to get response to Keep-Alive!");
            callback(nullptr);
        }
        
        RtspResponse* resp = nullptr;
        try {
            resp = new RtspResponse(raw_resp);
        } catch (const std::exception& e) {
            ERROR("Invalid KeepaliveResponse: " << e.what());
            resp = nullptr;
        }
        
        delete raw_resp;
        
        callback(resp);
    });
    
    delete keep_alive;
}

void Surge::RtspClient::StopClient() {
    Abort();
    
    if (m_transport->IsRunning()) {

        // non empty session token we should teardown
        if (!m_session.empty()) {
            Teardown();
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

    long long int time_last_packet_was_processed = SurgeUtil::currentTimeMilliseconds();

    m_transport->SetRtpCallback([&](RtpPacket* packet) {
        if (packet != nullptr) {
            ProcessRtpPacket(packet);
        
            delete packet;

            long long int test = SurgeUtil::currentTimeMilliseconds();
            time_last_packet_was_processed = test;
        }
    });
    
    while (true) {
        auto firedEvents = SurgeUtil::WaitableEvents::WaitFor({m_thread.StopRequested()}, 1000);        

        // STOP
        if (SurgeUtil::WaitableEvents::IsContainedIn(firedEvents, *(m_thread.StopRequested()))) {
            DEBUG("RtspClient - Stop Requested.");
            break;
        }

        // TIMEOUT
        int64_t timeout_delta_ms = SurgeUtil::currentTimeMilliseconds() - time_last_packet_was_processed;
        int maxTimeBetweenFrames = m_timeout + m_sessionDescription.GetTimeoutOffset();
        bool client_did_timeout = m_isPlaying && timeout_delta_ms >= maxTimeBetweenFrames;
        if (client_did_timeout) {
            ERROR("No processed packets in the last " << maxTimeBetweenFrames << "(ms). Issueing timeout signal.");
            NotifyDelegateTimeout();
            break;
        }

        // KEEP ALIVE
        int64_t delta = SurgeUtil::currentTimeMilliseconds() - m_lastKeepAliveMs;
        int64_t delta_seconds = delta / 1000;
        bool need_keep_alive = delta_seconds >= static_cast<int64_t>(m_keeepAliveIntervalInSeconds * 0.9);
        if (need_keep_alive) {
            DEBUG("Sending Keep Alive for session: " << m_url);

            KeepAlive([&](RtspResponse *resp) {
                if (resp == nullptr) {
                    // notify delegate via error dispatcher
                    ERROR("Failed to get response to keep alive");
                    NotifyDelegateTimeout();
                    m_thread.Stop();
                } else if (!resp->Ok()) {
                    ERROR("Failed Keep-Alive request: " << resp->GetCode());
                    delete resp;
                    NotifyDelegateTimeout();
                    m_thread.Stop();
                } else {
                    m_lastKeepAliveMs = SurgeUtil::currentTimeMilliseconds();
                    delete resp;
                }
            });
        }
    }

    m_transport->SetRtpCallback([&](RtpPacket* packet) { });
    INFO("Rtsp Client is Finished");
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
    
    CheckIfFrameShouldBeDropped(packet);
    
    if (!packet->IsMarked()) {
        return;
    }

    NotifyDelegateOfAvailableFrame();
    
    ClearFrameBuffer();
}

void Surge::RtspClient::CheckIfFrameShouldBeDropped(const Surge::RtpPacket* packet) {
    if (m_sessionDescription.GetType() == H264) {
        return;
    }
    
    if (packet->GetSequenceNumber() != ++lastPacket) {
        if (lastPacket != 1) {
            ERROR("PACKET LOST, FRAME DROPPED");
            DropNextFrame();
        }

        lastPacket = packet->GetSequenceNumber();
        missedPackets++;
    } else {
        successfulPackets++;
    }
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
        
        StopStream();
        m_transport->StopRunning();
    }

    m_abortWait.Reset();
    
    SurgeUtil::Url url_model(url);
    
    std::string host = url_model.GetHost();
    int port = url_model.GetPort();
    m_transport->RtspTcpOpen(host, port, [&](int result) {
        if (result == 0) {
            INFO("RUNNING THREAD");
            m_transport->StartRunning();
        } else {
            ERROR("Did not start the Transport thread.");
        }
        
        callback(result);
    });
}
